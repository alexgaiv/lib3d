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
	types = NULL;
}

void shared_traits<ProgramObject>::Uniforms::free() {
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
	if (ptr->knownUniforms.mvp_matrix != -1) {
		if (!rc->mvpComputed) {
			rc->mvpMatrix = rc->projection * rc->modelview;
			rc->mvpComputed = true;
		}
		glUniformMatrix4fv(ptr->knownUniforms.mvp_matrix, 1, GL_FALSE, rc->mvpMatrix.data);
	}
}

void ProgramObject::updateNorm()
{
	if (ptr->knownUniforms.normal_matrix != -1) {
		if (!rc->normComputed) {
			rc->normalMatrix = rc->modelview.GetInverse().GetTranspose();
			rc->normComputed = true;
		}
		glUniformMatrix4fv(ptr->knownUniforms.normal_matrix, 1, GL_FALSE, rc->normalMatrix.data);
	}
}

void ProgramObject::updateMatrices()
{
	if (ptr->fUpdateMV || ptr->fUpdateProj)
		updateMVP();

	if (ptr->fUpdateMV) {
		glUniformMatrix4fv(ptr->knownUniforms.modelView_matrix, 1, GL_FALSE, rc->modelview.data);
		updateNorm();
		ptr->fUpdateMV = false;
	}
	if (ptr->fUpdateProj) {
		glUniformMatrix4fv(ptr->knownUniforms.projection_matrix, 1, GL_FALSE, rc->projection.data);
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
		KnownUniforms &knownUniforms = ptr->knownUniforms;

		uniforms.free();
		glGetProgramiv(ptr->handle, GL_ACTIVE_UNIFORMS, &uniforms.count);
		if (uniforms.count != 0) {
			int maxLen = 0;
			glGetProgramiv(ptr->handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);

			uniforms.types = new GLenum[uniforms.count];

			GLint size = 0;
			char buf[1];
			for (int i = 0; i < uniforms.count; i++) {
				glGetActiveUniform(ptr->handle, i, 1, NULL, &size, &uniforms.types[i], buf);
			}

			knownUniforms.modelView_matrix = GetUniformLocation("ModelView");
			knownUniforms.projection_matrix = GetUniformLocation("Projection");
			knownUniforms.normal_matrix = GetUniformLocation("NormalMatrix");
			knownUniforms.mvp_matrix = GetUniformLocation("ModelViewProjection");
			knownUniforms.mtl_ambient = GetUniformLocation("Material.ambient");
			knownUniforms.mtl_diffuse = GetUniformLocation("Material.diffuse");
			knownUniforms.mtl_specular = GetUniformLocation("Material.specular");
			knownUniforms.mtl_shininess = GetUniformLocation("Material.shininess");
			knownUniforms.mtl_useDiffuseMap = GetUniformLocation("Material.useDiffuseMap");
			knownUniforms.mtl_useSpecularMap = GetUniformLocation("Material.useSpecularMap");
			knownUniforms.mtl_useNormalMap = GetUniformLocation("Material.useNormalMap");
			knownUniforms.mtl_useOpacityMask = GetUniformLocation("Material.useOpacityMask");
			knownUniforms.mtl_mode = GetUniformLocation("Material.mode");
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

void ProgramObject::Uniform(const char *name, float v0) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform1f(i, v0); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform2f(i, v0, v1); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform3f(i, v0, v1, v2); }
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2, float v3) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform4f(i, v0, v1, v2, v3); }
}
void ProgramObject::Uniform(const char *name, int v0) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform1i(i, v0); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform2i(i, v0, v1); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform3i(i, v0, v1, v2); }
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2, int v3) {
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) { Use(); glUniform4i(i, v0, v1, v2, v3); }
}
void ProgramObject::Uniform(const char *name, int count, const float *value) {
	int i = glGetUniformLocation(ptr->handle, name);
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
	int i = glGetUniformLocation(ptr->handle, name);
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
	int i = glGetUniformLocation(ptr->handle, name);
	if (i != -1) {
		Use();
		GLenum t = ptr->uniforms.types[i];
		if (t == GL_FLOAT_MAT4) glUniformMatrix4fv(i, count, transpose, v);
		else if (t == GL_FLOAT_MAT3) glUniformMatrix3fv(i, count, transpose, v);
		else glUniformMatrix2fv(i, count, transpose, v);
	}
}