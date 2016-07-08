#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include "common.h"
#include "sharedptr.h"

class GLRenderingContext;
class VertexBuffer;
class VertexArrayObject;

enum VertexAttribute
{
	VA_XYZ = 1,
	VA_NORMAL = 2,
	VA_TEXCOORD = 4,
	VA_TANGENTS_BINORMALS = 8
};

template<>
class shared_traits<VertexArrayObject>
{
public:
	GLuint vao;
	shared_traits() { glGenVertexArrays(1, &vao); }
	~shared_traits() { glDeleteVertexArrays(1, &vao); }
};

class VertexArrayObject : public Shared<VertexArrayObject>
{
public:
	GLuint Handle() const { return ptr->vao; }
	void Bind() { glBindVertexArray(ptr->vao); }
	void Unbind() { glBindVertexArray(0); }

	void EnableAttribs(int vaFlags);
	void EnableVertexAttrib(int index) { glEnableVertexAttribArray(index); }
	void DisableVertexAttrib(int index) { glDisableVertexAttribArray(index); }
	void VertexAttribDivisor(int index, int divisor) { glVertexAttribDivisor(index, divisor); }
};

template<>
class shared_traits<VertexBuffer>
{
public:
	GLuint id;
	int size;
	BYTE *data;
		
	shared_traits() : id(0), size(0), data(NULL) { }
	~shared_traits() {
		if (GLEW_ARB_vertex_buffer_object)
			glDeleteBuffers(1, &id);
		else delete [] data;
	}
};

class VertexBuffer : public Shared<VertexBuffer>
{
public:
	VertexBuffer(GLRenderingContext *rc, GLenum target);

	GLuint GetId() const { return ptr->id; }
	GLenum GetTarget() const { return target; }
	void SetTarget(GLenum target) { this->target = target; }

	void AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized = GL_FALSE, GLsizei stride = 0, GLubyte offset = 0) const;

	void VertexPointer(GLint size, GLenum type, GLsizei stride) const;
	void NormalPointer(GLenum type, GLsizei stride) const;
	void TexCoordPointer(GLint size, GLenum type, GLsizei stride) const;

	void DrawArrays(GLenum mode, GLint first, GLsizei count);
	void DrawElements(GLenum mode, GLsizei count, GLenum type, int offset = 0);
	void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);
	void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizei instanceCount, int offset = 0);

	void Bind() const { glBindBuffer(target, ptr->id); }
	void Unbind() const { glBindBuffer(target, 0); }

	void SetData(GLsizeiptr size, const void *data, GLenum usage);
	void SetSubData(GLintptr offset, GLsizeiptr size, const void *data);
	void GetSubData(GLintptr offset, GLsizeiptr size, void *data) const;
	int GetSize() const { return ptr->size; }
	GLenum GetUsage() const;

	void *Map(GLenum access) const;
	bool Unmap() const;

	void CloneTo(VertexBuffer &vb) const;
private:
	GLRenderingContext *rc;
	GLenum target;
};

#endif // _VERTEX_BUFFER_H