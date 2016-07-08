#include "vertexbuffer.h"
#include "shader.h"
#include "glcontext.h"

void VertexArrayObject::EnableAttribs(int flags)
{
	Bind();
	if (flags & VA_XYZ)
		glEnableVertexAttribArray(AttribLocation::Vertex);
	if (flags & VA_NORMAL)
		glEnableVertexAttribArray(AttribLocation::Normal);
	if (flags & VA_TEXCOORD)
		glEnableVertexAttribArray(AttribLocation::TexCoord);
	if (flags & VA_TANGENTS_BINORMALS) {
		glEnableVertexAttribArray(AttribLocation::Tangent);
		glEnableVertexAttribArray(AttribLocation::Binormal);
	}
}

VertexBuffer::VertexBuffer(GLRenderingContext *rc, GLenum target)
	: rc(rc), target(target)
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
	ProgramObject *p = rc->GetCurProgram();
	if (p) p->updateMatrices();
	glDrawArrays(mode, first, count);
}

void VertexBuffer::DrawElements(GLenum mode, GLsizei count, GLenum type, int offset)
{
	ProgramObject *p = rc->GetCurProgram();
	if (p) p->updateMatrices();

	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glDrawElements(mode, count, type, (void *)offset);
	}
	else {
		glDrawElements(mode, count, type, ptr->data + offset);
	}
}

void VertexBuffer::DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	ProgramObject *p = rc->GetCurProgram();
	if (p) p->updateMatrices();
	Bind();
	glDrawArraysInstanced(mode, first, count, instanceCount);
}

void VertexBuffer::DrawElementsInstanced(GLenum mode, GLsizei count,
	GLenum type, GLsizei instanceCount, int offset)
{
	ProgramObject *p = rc->GetCurProgram();
	if (p) p->updateMatrices();
	Bind();
	glDrawElementsInstanced(mode, count, type, (void *)offset, instanceCount);
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage)
{
	ptr->size = size;
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glBufferData(target, size, data, usage);
	}
	else {
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