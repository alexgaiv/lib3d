#include "vertexbuffer.h"

VertexBuffer::VertexBuffer() : target(0)
{
	glGenBuffers(1, &id);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &id);
}

void VertexBuffer::Bind(GLenum target) {
	this->target = target;
	glBindBuffer(target, id);
}

void VertexBuffer::Unbind(GLenum target) {
	glBindBuffer(target, 0);
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage) {
	glBindBuffer(target, id);
	glBufferData(target, size, data, usage);
}

void VertexBuffer::SetSubData(GLintptr offset, GLsizeiptr size, const void *data) {
	glBindBuffer(target, id);
	glBufferSubData(target, offset, size, data);
}

void VertexBuffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) {
	glBindBuffer(target, id);
	glGetBufferSubData(target, offset, size, data);
}

int VertexBuffer::GetSize() {
	int size;
	glBindBuffer(target, id);
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
	return size;
}

void *VertexBuffer::Map(GLenum access) {
	glBindBuffer(target, id);
	return glMapBuffer(target, access);
}

bool VertexBuffer::Unmap() {
	glBindBuffer(target, id);
	return glUnmapBuffer(target) == GL_TRUE;
}