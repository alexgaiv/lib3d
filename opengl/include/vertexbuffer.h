#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include "common.h"

class VertexBuffer
{
public:
	VertexBuffer(GLenum target = GL_ARRAY_BUFFER);
	~VertexBuffer();

	GLuint GetId() { return id; }

	void AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized = GL_FALSE, GLsizei stride = 0, GLubyte offset = 0);

	void Bind(GLenum target);
	void Unbind(GLenum target);

	void SetData(GLsizeiptr size, const void *data, GLenum usage);
	void SetSubData(GLintptr offset, GLsizeiptr size, const void *data);
	void GetSubData(GLintptr offset, GLsizeiptr size, void *data);
	int GetSize();

	void *Map(GLenum access);
	bool Unmap();
private:
	GLuint id;
	GLenum target;
};

#endif // _VERTEX_BUFFER_H