#include "vertexbuffer.h"
#include "shader.h"

VertexBuffer::VertexBuffer(GLRenderingContext *rc, GLenum target)
	: rc(rc), target(target), ptr(new Shared)
{
	if (GLEW_ARB_vertex_buffer_object)
		glGenBuffers(1, &ptr->id);
}

void VertexBuffer::AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized, GLsizei stride, GLubyte offset) const
{
	Bind();
	glVertexAttribPointer(index, size, type, normalized, stride, (void *)offset);
}

void VertexBuffer::VertexPointer(GLint size, GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glVertexPointer(size, type, stride, 0);
	}
	else {
		glVertexPointer(size, type, stride, ptr->data);
	}
}

void VertexBuffer::NormalPointer(GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glNormalPointer(type, stride, 0);
	}
	else {
		glNormalPointer(type, stride, ptr->data);
	}
}

void VertexBuffer::TexCoordPointer(GLint size, GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glTexCoordPointer(size, type, stride, 0);
	}
	else {
		glTexCoordPointer(size, type, stride, ptr->data);
	}
}

void VertexBuffer::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
	if (GLEW_ARB_vertex_buffer_object) {
		ProgramObject *p = rc->GetCurProgram();
		if (p) p->updateMatrices();
	}
	glDrawArrays(mode, first, count);
}

void VertexBuffer::DrawElements(GLenum mode, GLsizei count, GLenum type, int first)
{
	if (GLEW_ARB_vertex_buffer_object)
	{
		ProgramObject *p = rc->GetCurProgram();
		if (p) p->updateMatrices();

		Bind();
		glDrawElements(mode, count, type, (void *)first);
	}
	else {
		glDrawElements(mode, count, type, ptr->data);
	}
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage)
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glBufferData(target, size, data, usage);
	}
	else {
		ptr->size = size;
		delete [] ptr->data;
		ptr->data = new BYTE[size];
		memcpy(ptr->data, data, size);
	}
}

void VertexBuffer::SetSubData(GLintptr offset, GLsizeiptr size, const void *data)
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glBufferSubData(target, offset, size, data);
	}
	else if (offset < ptr->size) {
		memcpy(ptr->data + offset, data, min(size, ptr->size - offset));
	}
}

void VertexBuffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glGetBufferSubData(target, offset, size, data);
	}
	else if (offset < ptr->size) {
		memcpy(data, ptr->data + offset, min(size, ptr->size - offset));
	}
}

int VertexBuffer::GetSize() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		int size = 0;
		Bind();
		glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
		return size;
	}
	return ptr->size;
}

GLenum VertexBuffer::GetUsage() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		int usage = 0;
		glGetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);
		return (GLenum)usage;
	}
	return GL_STATIC_DRAW;
}

void *VertexBuffer::Map(GLenum access) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		return glMapBuffer(target, access);
	}
	else return ptr->data;
}

bool VertexBuffer::Unmap() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		return glUnmapBuffer(target) == GL_TRUE;
	}
	return true;
}

void VertexBuffer::CloneTo(VertexBuffer &vb) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		BYTE *data = (BYTE *)Map(GL_READ_ONLY);
		if (data) {
			vb.SetData(GetSize(), data, GetUsage());
			Unmap();
		}
	}
	else {
		delete [] vb.ptr->data;
		vb.ptr->size = ptr->size;
		vb.ptr->data = new BYTE[ptr->size];
		memcpy(vb.ptr->data, ptr->data, ptr->size);
	}
}