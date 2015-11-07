#ifndef _SHADER_H_
#define _SHADER_H_

#include "common.h"
#include <fstream>
#include <vector>
#include <string>

class Shader
{
public:
	Shader(GLenum type);
	Shader(GLenum type, const char *path);
	~Shader();

	GLuint Handle() const { return handle; }
	bool IsCompiled() const { return compiled; }
	bool CompileFile(const char *filename);
private:
	GLuint handle;
	bool compiled;
};

class ProgramObject
{
public:
	ProgramObject();
	ProgramObject(const ProgramObject &p);
	ProgramObject(const char *vertPath, const char *fragPath);
	~ProgramObject();
	ProgramObject &operator=(const ProgramObject &p);

	bool IsBad() const { return bad; }
	GLuint Handle() const { return handle; }
	bool isLinked() const { return linked; }
	void AttachShader(const Shader &shader);
	void DetachShader(const Shader &shader);
	bool Link();
	void Use();

	void BindAttribLocation(GLuint index, const char *name);
	GLuint GetUniformLocation(const char *name);

	void ModelView(const float *v);
	void Projection(const float *v);
	void NormalMatrix(const float *v);

	void Uniform(const char *name, float v0);
	void Uniform(const char *name, float v0, float v1);
	void Uniform(const char *name, float v0, float v1, float v2);
	void Uniform(const char *name, float v0, float v1, float v2, float v3);
	void Uniform(const char *name, int v0);
	void Uniform(const char *name, int v0, int v1);
	void Uniform(const char *name, int v0, int v1, int v2);
	void Uniform(const char *name, int v0, int v1, int v2, int v3);
	void Uniform(const char *name, int count, const float *value);
	void Uniform(const char *name, int count, const int *value);
	void UniformMatrix(const char *name, int count, bool transpose, const float *v);
private:
	struct {
		int count;
		char **names;
		GLenum *types;
		int mvLoc, projLoc, normLoc;

		void free() {
			if (!count) return;
			for (int i = 0; i < count; i++) {
				delete [] names[i];
			}
			delete [] names;
			delete [] types;
		}
	} uniforms;

	GLuint handle;
	bool linked;
	bool bad;

	void clone(const ProgramObject &p);
	int lookup(const char *name) {
		for (int i = 0; i < uniforms.count; i++) {
			if (!strcmp(name, uniforms.names[i]))
				return i;
		}
		return -1;
	}

	bool is2(GLenum t) {
		return t == GL_FLOAT_VEC2 || t == GL_INT_VEC2 ||
			t == GL_UNSIGNED_INT_VEC2 || t == GL_BOOL_VEC2;
	}
	bool is3(GLenum t) {
		return t == GL_FLOAT_VEC3 || t == GL_INT_VEC3 ||
			t == GL_UNSIGNED_INT_VEC3 || t == GL_BOOL_VEC3;
	}
	bool is4(GLenum t) {
		return t == GL_FLOAT_VEC4 || t == GL_INT_VEC4 ||
			t == GL_UNSIGNED_INT_VEC4 || t == GL_BOOL_VEC4;
	}
};

#include "global.h"

#endif // _SHADER_H_