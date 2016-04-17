#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include "common.h"
#include "glcontext.h"
#include "sharedptr.h"

class GLRenderingContext;

class VertexBuffer
{
private:
	struct Shared;
	my_shared_ptr<Shared> ptr;
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
	int GetSize() const { return size; }
	GLenum GetUsage() const;

	void *Map(GLenum access) const;
	bool Unmap() const;

	void CloneTo(VertexBuffer &vb) const;
private:
	struct Shared
	{
		GLuint id;
		BYTE *data;
		
		Shared() : id(0), data(NULL) { }
		~Shared() {
			if (GLEW_ARB_vertex_buffer_object)
				glDeleteBuffers(1, &id);
			else delete [] data;
		}
	};

	GLRenderingContext *rc;
	GLenum target;
	int size;
};

#endif // _VERTEX_BUFFER_H