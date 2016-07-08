#include "shader.h"
#include "glcontext.h"
#include <strsafe.h>

Shader::Shader(GLenum type) {
	ptr->handle = glCreateShader(type);
}

Shader::Shader(GLenum type, const char *path) {
	ptr->handle = glCreateShader(type);
	ptr->compiled = CompileFile(path);
}

bool Shader::CompileFile(const char *filename)
{
	std::ifstream hFile(filename);
	if (!hFile) return false;

	std::vector<std::string> lines;
	std::string line;
	while(std::getline(hFile, line))
		lines.push_back(line + "\n");
	hFile.close();

	int numLines = lines.size();
	const char **source = new const char*[numLines];
	for (int i = 0; i < numLines; i++) {
		source[i] = lines[i].c_str();
	}

	glShaderSource(ptr->handle, numLines, (const GLchar **)source, NULL);
	glCompileShader(ptr->handle);
	delete [] source;
	return log();
}

bool Shader::CompileSource(const char *source, int length)
{
	glShaderSource(ptr->handle, 1, &source, length ? &length : NULL);
	glCompileShader(ptr->handle);
	return log();
}

bool Shader::log()
{
	GLint isCompiled = 0;
	GLint logLen = 0;
	glGetShaderiv(ptr->handle, GL_COMPILE_STATUS, &isCompiled);
	glGetShaderiv(ptr->handle, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen != 0) {
		GLchar *infoLog = new GLchar[logLen];
		glGetShaderInfoLog(ptr->handle, logLen, &logLen, infoLog);
		OutputDebugString(infoLog);
		delete [] infoLog;
	}
	ptr->compiled = isCompiled ? true : false;
	return isCompiled == TRUE;
}

shared_traits<ProgramObject>::shared_traits()
	: rc(rc), handle(0), linked(false)
{
	handle = glCreateProgram();
	fUpdateMV = fUpdateProj = true;
}

shared_traits<ProgramObject>::~shared_traits()
{
	rc->DetachProgram(this);
	glDeleteProgram(handle);
}

shared_traits<ProgramObject>::Uniforms::Uniforms()
{
	count = 0;
	names = NULL;
	types = NULL;
	mvLoc = projLoc = mvpLoc = normLoc = 0;
}

void shared_traits<ProgramObject>::Uniforms::free() {
	if (count == 0) return;
	for (int i = 0; i < count; i++) {
		delete [] names[i];
	}
	delete [] names;
	delete [] types;
}

ProgramObject::ProgramObject(GLRenderingContext *rc) : rc(rc)
{
	ptr->rc = rc;
	rc->AttachProgram(ptr.Get());
}

ProgramObject::ProgramObject(GLRenderingContext *rc, const char *vertPath, const char *fragPath) : rc(rc)
{
	ptr->rc = rc;
	rc->AttachProgram(ptr.Get());

	Shader vertShader(GL_VERTEX_SHADER, vertPath);
	Shader fragShader(GL_FRAGMENT_SHADER, fragPath);

	if (vertShader.IsCompiled() && fragShader.IsCompiled()) {
		AttachShader(vertShader);
		AttachShader(fragShader);
		Link();
	}
}

void ProgramObject::AttachShader(const Shader &shader) {
	glAttachShader(ptr->handle, shader.Handle());
}

void ProgramObject::DetachShader(const Shader &shader) {
	glDetachShader(ptr->handle, shader.Handle());
}

void ProgramObject::updateMVP()
{
	if (HasMvpMatrix()) {
		if (!rc->mvpComputed) {
			rc->mvpMatrix = rc->projection * rc->modelview;
			rc->mvpComputed = true;
		}
		glUniformMatrix4fv(ptr->uniforms.mvpLoc, 1, GL_FALSE, rc->mvpMatrix.data);
	}
}

void ProgramObject::updateNorm()
{
	if (HasNormalMatrix()) {
		if (!rc->normComputed) {
			rc->normalMatrix = rc->modelview.GetInverse().GetTranspose();
			rc->normComputed = true;
		}
		glUniformMatrix4fv(ptr->uniforms.normLoc, 1, GL_FALSE, rc->normalMatrix.data);
	}
}

void ProgramObject::updateMatrices()
{
	if (ptr->fUpdateMV) {
		glUniformMatrix4fv(ptr->uniforms.mvLoc, 1, GL_FALSE, rc->modelview.data);
		updateMVP();
		updateNorm();
		ptr->fUpdateMV = false;
	}
	if (ptr->fUpdateProj) {
		glUniformMatrix4fv(ptr->uniforms.projLoc, 1, GL_FALSE, rc->projection.data);
		updateMVP();
		ptr->fUpdateProj = false;
	}
}

void ProgramObject::Use()
{
	if (!rc->curProgram || rc->curProgram->ptr->handle != ptr->handle) {
		rc->curProgram = this;
		glUseProgram(ptr->handle);
	}
}

bool ProgramObject::Link()
{
	GLint isLinked = 0;
	GLint logLen = 0;

	BindAttribLocation(AttribLocation::Vertex, "Vertex");
	BindAttribLocation(AttribLocation::Normal, "Normal");
	BindAttribLocation(AttribLocation::TexCoord, "TexCoord");
	BindAttribLocation(AttribLocation::Tangent, "Tangent");
	BindAttribLocation(AttribLocation::Binormal, "Binormal");

	glLinkProgram(ptr->handle);
	glGetProgramiv(ptr->handle, GL_LINK_STATUS, &isLinked);
	glGetProgramiv(ptr->handle, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen != 0)
	{
		GLchar *infoLog = new GLchar[logLen];
		glGetProgramInfoLog(ptr->handle, logLen, &logLen, infoLog);
		OutputDebugString(infoLog);
		delete [] infoLog;
	}

	ptr->linked = isLinked == TRUE;
	if (ptr->linked)
	{
		shared_traits<ProgramObject>::Uniforms &uniforms = ptr->uniforms;
		uniforms.free();
		glGetProgramiv(ptr->handle, GL_ACTIVE_UNIFORMS, &uniforms.count);
		if (uniforms.count != 0) {
			int maxLen = 0;
			glGetProgramiv(ptr->handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);

			uniforms.names = new char *[uniforms.count];
			uniforms.types = new GLenum[uniforms.count];

			GLint size = 0;
			for (int i = 0; i < uniforms.count; i++) {
				uniforms.names[i] = new char[maxLen];
				glGetActiveUniform(ptr->handle, i, maxLen, NULL, &size, &uniforms.types[i], uniforms.names[i]);
			}
			uniforms.mvLoc = GetUniformLocation("ModelView");
			uniforms.projLoc = GetUniformLocation("Projection");
			uniforms.normLoc = GetUniformLocation("NormalMatrix");
			uniforms.mvpLoc = GetUniformLocation("ModelViewProjection");
		}
	}

	return ptr->linked;
}

GLint ProgramObject::GetAttribLocation(const char *name) {
	return glGetAttribLocation(ptr->handle, name);
}

void ProgramObject::BindAttribLocation(GLuint index, const char *name) {
	glBindAttribLocation(ptr->handle, index, name);
}

GLuint ProgramObject::GetUniformLocation(const char *name) {
	return glGetUniformLocation(ptr->handle, name);
}

void ProgramObject::ModelView(const Matrix44f &m) {
	if (ptr->uniforms.mvLoc != -1) {
		Use();
		glUniformMatrix4fv(ptr->uniforms.mvLoc, 1, GL_FALSE, m.data);
	}
}

void ProgramObject::Projection(const Matrix44f &m) {
	if (ptr->uniforms.projLoc != -1) {
		Use();
		glUniformMatrix4fv(ptr->uniforms.projLoc, 1, GL_FALSE, m.data);
	}
}

void ProgramObject::ModelViewProjection(const Matrix44f &m) {
	if (ptr->uniforms.mvpLoc != -1) {
		Use();
		glUniformMatrix4fv(ptr->uniforms.mvpLoc, 1, GL_FALSE, m.data);
	}
}

void ProgramObject::NormalMatrix(const Matrix44f &m) {
	if (ptr->uniforms.normLoc != -1) {
		Use();
		glUniformMatrix4fv(ptr->uniforms.normLoc, 1, GL_FALSE, m.data);
	}
}

void ProgramObject::Uniform(const char *name, float v0) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform1f(i, v0); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform2f(i, v0, v1); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform3f(i, v0, v1, v2); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2, float v3) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform4f(i, v0, v1, v2, v3); }
}
void ProgramObject::Uniform(const char *name, int v0) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform1i(i, v0); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform2i(i, v0, v1); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform3i(i, v0, v1, v2); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2, int v3) {
	int i = lookup(name);
	if (i != -1) { Use(); glUniform4i(i, v0, v1, v2, v3); }
}
void ProgramObject::Uniform(const char *name, int count, const float *value) {
	int i = lookup(name);
	if (i != -1) {
		Use();
		GLenum t = ptr->uniforms.types[i];
		if (is4(t)) glUniform4fv(i, count, value);
		else if (is3(t)) glUniform3fv(i, count, value);
		else if (is2(t)) glUniform2fv(i, count, value);
		else glUniform1fv(i, count, value);
	}
}
void ProgramObject::Uniform(const char *name, int count, const int *value) {
	int i = lookup(name);
	if (i != -1) {
		Use();
		GLenum t = ptr->uniforms.types[i];
		if (is4(t)) glUniform4iv(i, count, value);
		else if (is3(t)) glUniform3iv(i, count, value);
		else if (is2(t)) glUniform2iv(i, count, value);
		else glUniform1iv(i, count, value);
	}
}

void ProgramObject::UniformMatrix(const char *name, int count, bool transpose, const float *v)
{
	int i = lookup(name);
	if (i != -1) {
		Use();
		GLenum t = ptr->uniforms.types[i];
		if (t == GL_FLOAT_MAT4) glUniformMatrix4fv(i, count, transpose, v);
		else if (t == GL_FLOAT_MAT3) glUniformMatrix3fv(i, count, transpose, v);
		else glUniformMatrix2fv(i, count, transpose, v);
	}
}