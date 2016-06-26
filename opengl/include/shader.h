#ifndef _SHADER_H_
#define _SHADER_H_

#include "common.h"
#include "datatypes.h"
#include "glcontext.h"
#include "vertexbuffer.h"
#include "sharedptr.h"
#include <fstream>
#include <vector>
#include <string>

class GLRenderingContext;
class Shader;
class ProgramObject;

enum AttribLocation
{
	Vertex = 0,
	Normal = 1,
	TexCoord = 2,
	Tangent = 3,
	Binormal = 4
};

template<>
class shared_traits<Shader>
{
public:
	GLuint handle;
	bool compiled;
	shared_traits() : handle(0), compiled(false) { }
	~shared_traits() { glDeleteShader(handle); }
};

class Shader : public Shared<Shader>
{
public:
	Shader(GLenum type);
	Shader(GLenum type, const char *path);

	GLuint Handle() const { return ptr->handle; }
	bool IsCompiled() const { return ptr->compiled; }
	bool CompileFile(const char *filename);
	bool CompileSource(const char *source, int length = 0);
private:
	bool log();
};

template<>
class shared_traits<ProgramObject>
{
public:
	GLRenderingContext *rc;
	GLuint handle;
	bool linked;
	bool fUpdateMV, fUpdateProj;

	shared_traits();
	~shared_traits();

	struct Uniforms
	{
		int count;
		char **names;
		GLenum *types;
		int mvLoc, projLoc, mvpLoc, normLoc;

		Uniforms();
		~Uniforms() { free(); }
		void free();
	} uniforms;
};

class ProgramObject : public Shared<ProgramObject>
{
public:
	ProgramObject(GLRenderingContext *rc);
	ProgramObject(GLRenderingContext *rc, const char *vertPath, const char *fragPath);

	GLuint Handle() const { return ptr->handle; }
	bool IsLinked() const { return ptr->linked; }
	void AttachShader(const Shader &shader);
	void DetachShader(const Shader &shader);
	bool Link();
	void Use();

	GLint GetAttribLocation(const char *name);
	void BindAttribLocation(GLuint index, const char *name);
	GLuint GetUniformLocation(const char *name);

	bool HasMvpMatrix() const { return ptr->uniforms.mvpLoc != -1; }
	bool HasNormalMatrix() const { return ptr->uniforms.normLoc != -1; }

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
	friend class GLRenderingContext;
	friend class VertexBuffer;
	
	GLRenderingContext *rc;
	
	void updateMatrices();
	void updateMVP();
	void updateNorm();

	int lookup(const char *name) {
		for (int i = 0; i < ptr->uniforms.count; i++) {
			if (!strcmp(name, ptr->uniforms.names[i]))
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

#endif // _SHADER_H_