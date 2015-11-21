#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "common.h"
#include "texture.h"

class Renderbuffer
{
public:
	Renderbuffer();
	~Renderbuffer();

	void AllocateStorage(GLenum internalFormat, GLsizei width, GLsizei height);
	void AllocateMultisampleStorage(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);

	GLuint GetId() const { return id; }
	void Bind() {
		glBindRenderbuffer(GL_RENDERBUFFER, id);
	}
private:
	GLuint id;
};

class Framebuffer
{
public:
	Framebuffer(GLenum target = GL_FRAMEBUFFER);
	~Framebuffer();

	void AttachTexture(GLenum attachment, const BaseTexture &texture, GLint level = 0);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer &rb);
	void AttachDepthRenderbuffer(GLsizei width, GLsizei height);

	GLuint GetId() const { return id; }
	void Bind() {
		glBindFramebuffer(target, id);
	}
	void Unbind() {
		glBindFramebuffer(target, 0);
	}
	GLenum GetStatus() const {
		return glCheckFramebufferStatus(target);
	}
	bool IsComplete() const {
		return GetStatus() == GL_FRAMEBUFFER_COMPLETE;
	}
private:
	GLuint id;
	GLenum target;
	Renderbuffer depthBuffer;
};

#endif // _FRAMEBUFFER_H