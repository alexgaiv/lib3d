#include "shader.h"
#include "global.h"
#include <strsafe.h>

Shader::Shader(GLenum type) {
	handle = glCreateShader(type);
}

Shader::Shader(GLenum type, const char *path) {
	handle = glCreateShader(type);
	compiled = CompileFile(path);
}

Shader::~Shader() {
	glDeleteShader(handle);
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

	glShaderSource(handle, numLines, (const GLchar **)source, NULL);
	glCompileShader(handle);
	delete[] source;
		
	GLint isCompiled = 0;
	GLint logLen = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &isCompiled);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen != 0) {
		GLchar *infoLog = new GLchar[logLen];
		glGetShaderInfoLog(handle, logLen, &logLen, infoLog);
		OutputDebugString(infoLog);
		delete[] infoLog;
	}
	return isCompiled == TRUE;
}

ProgramObject::ProgramObject() {
	Global::AttachProgram(this);
	memset(&uniforms, 0, sizeof(uniforms));
	linked = bad = false;
	handle = glCreateProgram();
}

ProgramObject::ProgramObject(const ProgramObject &p) {
	clone(p);
}

ProgramObject::ProgramObject(const char *vertPath, const char *fragPath)
{
	Global::AttachProgram(this);
	memset(&uniforms, 0, sizeof(uniforms));
	linked = bad = false;
	handle = glCreateProgram();

	Shader vertShader(GL_VERTEX_SHADER, vertPath);
	Shader fragShader(GL_FRAGMENT_SHADER, fragPath);

	if (vertShader.IsCompiled() && fragShader.IsCompiled()) {
		AttachShader(vertShader);
		AttachShader(fragShader);
	}
	else bad = true;
	Link();
}

ProgramObject::~ProgramObject() {
	Global::DetachProgram(this);
	glDeleteProgram(handle);
	uniforms.free();
}

ProgramObject &ProgramObject::operator=(const ProgramObject &p)
{
	uniforms.free();
	clone(p);
	return *this;
}

void ProgramObject::clone(const ProgramObject &p)
{
	memcpy(&uniforms, &p.uniforms, sizeof(uniforms));
	handle = p.handle;
	linked = p.linked;
	bad = p.bad;

	int c = p.uniforms.count;
	if (c != 0) {
		uniforms.names = new char *[c];
		uniforms.types = new GLenum[c];
		for (int i = 0; i < c; i++) {
			int len = strlen(p.uniforms.names[i]) + 1;
			uniforms.names[i] = new char[len];
			uniforms.types[i] = p.uniforms.types[i];
			StringCchCopyA(uniforms.names[i], len, p.uniforms.names[i]);
		}
	}
}

void ProgramObject::AttachShader(const Shader &shader) {
	glAttachShader(handle, shader.Handle());
}

void ProgramObject::DetachShader(const Shader &shader) {
	glDetachShader(handle, shader.Handle());
}

void ProgramObject::Use() {
	if (linked) glUseProgram(handle);
}

bool ProgramObject::Link()
{
	GLint isLinked = 0;
	GLint logLen = 0;

	BindAttribLocation(AttribsLocations.Vertex, "Vertex");
	BindAttribLocation(AttribsLocations.Normal, "Normal");
	BindAttribLocation(AttribsLocations.TexCoord, "TexCoord");

	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &isLinked);
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen != 0)
	{
		GLchar *infoLog = new GLchar[logLen];
		glGetProgramInfoLog(handle, logLen, &logLen, infoLog);
		OutputDebugString(infoLog);
		delete[] infoLog;
	}
	linked = isLinked == TRUE;

	if (linked)
	{
		bad = false;
		uniforms.free();

		glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &uniforms.count);
		if (uniforms.count != 0) {
			int maxLen = 0;
			glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
			uniforms.names = new char *[uniforms.count];
			uniforms.types = new GLenum[uniforms.count];

			GLint size = 0;
			for (int i = 0; i < uniforms.count; i++) {
				uniforms.names[i] = new char[maxLen];
				glGetActiveUniform(handle, i, maxLen, NULL, &size, &uniforms.types[i], uniforms.names[i]);
			}
			uniforms.mvLoc = GetUniformLocation("ModelView");
			uniforms.projLoc = GetUniformLocation("Projection");
			uniforms.normLoc = GetUniformLocation("NormalMatrix");
		}
	}

	return linked;
}

void ProgramObject::BindAttribLocation(GLuint index, const char *name) {
	glBindAttribLocation(handle, index, name);
}

GLuint ProgramObject::GetUniformLocation(const char *name) {
	return glGetUniformLocation(handle, name);
}

void ProgramObject::ModelView(const float *v) {
	glUniformMatrix4fv(uniforms.mvLoc, 1, GL_FALSE, v);
}

void ProgramObject::Projection(const float *v) {
	glUniformMatrix4fv(uniforms.projLoc, 1, GL_FALSE, v);
}

void ProgramObject::NormalMatrix(const float *v) {
	glUniformMatrix4fv(uniforms.normLoc, 1, GL_FALSE, v);
}

void ProgramObject::Uniform(const char *name, float v0) {
	int i = lookup(name);
	if (i != -1) glUniform1f(i, v0);
}
void ProgramObject::Uniform(const char *name, float v0, float v1) {
	int i = lookup(name);
	if (i != -1) glUniform2f(i, v0, v1);
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2) {
	int i = lookup(name);
	if (i != -1) glUniform3f(i, v0, v1, v2);
}
void ProgramObject::Uniform(const char *name, float v0, float v1, float v2, float v3) {
	int i = lookup(name);
	if (i != -1) glUniform4f(i, v0, v1, v2, v3);
}
void ProgramObject::Uniform(const char *name, int v0) {
	int i = lookup(name);
	if (i != -1) glUniform1i(i, v0);
}
void ProgramObject::Uniform(const char *name, int v0, int v1) {
	int i = lookup(name);
	if (i != -1) glUniform2i(i, v0, v1);
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2) {
	int i = lookup(name);
	if (i != -1) glUniform3i(i, v0, v1, v2);
}
void ProgramObject::Uniform(const char *name, int v0, int v1, int v2, int v3) {
	int i = lookup(name);
	if (i != -1) glUniform4i(i, v0, v1, v2, v3);
}
void ProgramObject::Uniform(const char *name, int count, const float *value) {
	int i = lookup(name);
	if (i != -1) {
		GLenum t = uniforms.types[i];
		if (is4(t)) glUniform4fv(i, count, value);
		else if (is3(t)) glUniform3fv(i, count, value);
		else if (is2(t)) glUniform2fv(i, count, value);
		else glUniform1fv(i, count, value);
	}
}
void ProgramObject::Uniform(const char *name, int count, const int *value) {
	int i = lookup(name);
	if (i != -1) {
		GLenum t = uniforms.types[i];
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
		GLenum t = uniforms.types[i];
		if (t == GL_FLOAT_MAT4) glUniformMatrix4fv(i, count, transpose, v);
		else if (t == GL_FLOAT_MAT3) glUniformMatrix3fv(i, count, transpose, v);
		else glUniformMatrix2fv(i, count, transpose, v);
	}
}