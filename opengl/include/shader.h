#ifndef _SHADER_H_
#define _SHADER_H_

#include "common.h"
#include "datatypes.h"
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
	bool CompileSource(const char *source);
private:
	GLuint handle;
	bool compiled;
	bool _log();
};

class ProgramObject
{
public:
	ProgramObject();
	ProgramObject(const ProgramObject &p);
	ProgramObject(const char *vertPath, const char *fragPath);
	~ProgramObject();
	ProgramObject &operator=(const ProgramObject &p);

	GLuint Handle() const { return handle; }
	bool IsLinked() const { return linked; }
	void AttachShader(const Shader &shader);
	void DetachShader(const Shader &shader);
	bool Link();
	void Use();

	GLint GetAttribLocation(const char *name);
	void BindAttribLocation(GLuint index, const char *name);
	GLuint GetUniformLocation(const char *name);

	bool HasMVP() const { return uniforms.mvpLoc != -1; }
	bool HasNormalMatrix() const { return uniforms.normLoc != -1; }

	void ModelView(const Matrix44f &m);
	void Projection(const Matrix44f &m);
	void ModelViewProjection(const Matrix44f &m);
	void NormalMatrix(const Matrix44f &m);

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
		int mvLoc, projLoc, mvpLoc, normLoc;

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