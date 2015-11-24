#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "common.h"
#include "datatypes.h"

#define TEX_OWN_ID GLuint(-1)

class BaseTexture
{
public:
	BaseTexture(GLenum target, GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_OWN_ID);

	GLuint GetId() const { return id; }
	void Bind();
	void Delete() {
		glDeleteTextures(1, &id);
	}

	GLenum GetTextureUnit() const { return textureUnit; }
	void SetTextureUnit(GLenum unit) { textureUnit = unit; }

	void SetFilters(GLint minFilter, GLint magFilter = -1);
	void SetWrapMode(GLint wrapS, GLint wrapT, GLint wrapR = -1);
	void SetBorderColor(Color4f color);
protected:
	GLuint id;
	GLenum target, textureUnit;
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
	Texture2D(GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_OWN_ID);
	Texture2D(const char *filename, GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_OWN_ID);

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
	CubeTexture(GLenum textureUnit = GL_TEXTURE0);
	CubeTexture(const char **sides, GLenum textureUnit = GL_TEXTURE0);

	bool IsLoaded() const { return loaded; }
	bool LoadFromTGA(const char **sides);
private:
	bool loaded;
};

#endif // _TEXTURE2D_H_