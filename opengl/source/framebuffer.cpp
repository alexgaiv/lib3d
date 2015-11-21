#include "framebuffer.h"

Renderbuffer::Renderbuffer() {
	glGenRenderbuffers(1, &id);
}

Renderbuffer::~Renderbuffer() {
	glDeleteRenderbuffers(1, &id);
}

void Renderbuffer::AllocateStorage(GLenum internalFormat, GLsizei width, GLsizei height) {
	Bind();
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

void Renderbuffer::AllocateMultisampleStorage(GLsizei samples,
	GLenum internalFormat, GLsizei width, GLsizei height)
{
	Bind();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
}

Framebuffer::Framebuffer(GLenum target) {
	this->target = target;
	glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &id);
}

void Framebuffer::AttachTexture(GLenum attachment, const BaseTexture &texture, GLint level)
{
	GLint bind_draw = 0, bind_read = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bind_draw);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &bind_read);
	Bind();
	glFramebufferTexture(target, attachment, texture.GetId(), level);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bind_draw);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, bind_read);
}

void Framebuffer::AttachRenderbuffer(GLenum attachment, const Renderbuffer &rb)
{
	GLint bind_draw = 0, bind_read = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &bind_draw);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &bind_read);
	Bind();
	glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, rb.GetId());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bind_draw);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, bind_read);
}

void Framebuffer::AttachDepthRenderbuffer(GLsizei width, GLsizei height)
{
	depthBuffer.AllocateStorage(GL_DEPTH_COMPONENT24, width, height);
	AttachRenderbuffer(GL_DEPTH_ATTACHMENT, depthBuffer);
}
