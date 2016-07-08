#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "common.h"
#include "sharedptr.h"
#include "texture.h"

class Renderbuffer;
class Framebuffer;

template<>
class shared_traits<Renderbuffer>
{
public:
	GLuint id;
	shared_traits() { glGenRenderbuffers(1, &id); }
	~shared_traits() { glDeleteRenderbuffers(1, &id);  }
};

class Renderbuffer : public Shared<Renderbuffer>
{
public:
	void AllocateStorage(GLenum internalFormat, GLsizei width, GLsizei height);
	void AllocateMultisampleStorage(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);

	GLuint GetId() const { return ptr->id; }
	void Bind() {
		glBindRenderbuffer(GL_RENDERBUFFER, ptr->id);
	}
};

template<>
class shared_traits<Framebuffer>
{
public:
	GLuint id;
	shared_traits() { glGenFramebuffers(1, &id); }
	~shared_traits() { glDeleteFramebuffers(1, &id); }
};

class Framebuffer : public Shared<Framebuffer>
{
public:
	Framebuffer(GLenum target = GL_FRAMEBUFFER);

	void AttachTexture(GLenum attachment, const BaseTexture &texture, GLint level = 0);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer &rb);

	GLuint GetId() const { return ptr->id; }
	void Bind() {
		glBindFramebuffer(target, ptr->id);
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
	GLenum target;
};

#endif // _FRAMEBUFFER_H