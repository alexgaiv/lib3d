#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "common.h"
#include "texture.h"
#include "sharedptr.h"

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
	struct Shared;
	my_shared_ptr<Shared> ptr;
public:
	Framebuffer(GLenum target = GL_FRAMEBUFFER);

	void AttachTexture(GLenum attachment, const BaseTexture &texture, GLint level = 0);
	void AttachRenderbuffer(GLenum attachment, const Renderbuffer &rb);
	void AttachDepthRenderbuffer(GLsizei width, GLsizei height);

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
	struct Shared
	{
		GLuint id;
		Shared() : id(0) { }
		~Shared() { glDeleteFramebuffers(1, &id); }
	};
	
	GLenum target;
	Renderbuffer depthBuffer;
};

#endif // _FRAMEBUFFER_H