#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "common.h"

class BaseTexture
{
public:
	BaseTexture(GLenum target, GLuint id = GLuint(-1));

	GLuint GetId() const { return id; }
	void Bind(GLenum textureUnit = GLenum(-1));
	void Delete() {
		glDeleteTextures(1, &id);
	}

	void SetFilters(GLint minFilter, GLint magFilter = -1);
	void SetWrapMode(GLint wrapS, GLint wrapT, GLint wrapR = -1);
protected:
	GLuint id;
	GLenum target, unitTarget;
	int width, height;
	int internalFormat, format;

	bool _loadFromTGA(const char *filename, BYTE *&data);
	void _texImage2D(GLenum target, BYTE *imageData);
private:
	void _read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes);
};

class Texture2D : public BaseTexture
{
public:
	Texture2D(GLuint id = GLuint(-1));
	Texture2D(const char *filename, GLuint id = GLuint(-1));

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	bool IsLoaded() const { return loaded; }
	bool LoadFromTGA(const char *filename);
	void BuildMipmaps();
	void SetTexImage(GLenum level, GLint internalFormat, GLsizei width, GLsizei height,
		GLint border, GLenum format, GLenum type, const GLvoid *data);
private:
	bool loaded;
};

class CubeTexture : public BaseTexture
{
public:
	CubeTexture();
	CubeTexture(const char **sides);

	bool IsLoaded() const { return loaded; }
	bool LoadFromTGA(const char **sides);
private:
	bool loaded;
};

#endif // _TEXTURE2D_H_