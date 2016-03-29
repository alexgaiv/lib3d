#include "texture.h"
#include "glcontext.h"
#include <new>

BaseTexture::BaseTexture(GLenum target, GLenum unit, GLuint id)
	: target(target), textureUnit(unit), ptr(new Shared)
{
	width = height = 0;
	internalFormat = format = 0;

	if (id == TEX_GENERATE_ID) {
		glGenTextures(1, &ptr->id);
		ptr->needDelete = true;
	} else {
		ptr->id = id;
		ptr->needDelete = false;
	}
}

void BaseTexture::Bind()
{
	if (glActiveTexture) glActiveTexture(textureUnit);
	glBindTexture(target, ptr->id);
}

void BaseTexture::SetFilters(GLint minFilter, GLint magFilter)
{
	Bind();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
}

void BaseTexture::SetWrapMode(GLint wrapS, GLint wrapT, GLint wrapR)
{
	Bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	if (wrapR != -1)
		glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
}

void BaseTexture::SetBorderColor(Color4f color)
{
	Bind();
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data);
}

void BaseTexture::BuildMipmaps() {
	if (glGenerateMipmap) {
		Bind();
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

void BaseTexture::read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes)
{
	DWORD bytesRead;
	BOOL success = ReadFile(hFile, lpBuffer, nNumBytes, &bytesRead, NULL);
	if (!success || bytesRead != nNumBytes)
		throw false;
}

bool BaseTexture::loadFromTGA(const char *filename, Image &img)
{
	img.LoadTga(filename);
	if (!img) return false;

	width = img.GetWidth();
	height = img.GetHeight();

	switch (img.GetDepth())
	{
	case 8:
		internalFormat = GL_LUMINANCE;
		format = GL_LUMINANCE;
		break;
	case 24:
		internalFormat = GL_RGB;
		format = GL_BGR;
		break;
	case 32:
		internalFormat = GL_RGBA;
		format = GL_BGRA;
		break;
	}
	return true;
}

void BaseTexture::texImage2D(GLenum target, BYTE *imageData)
{
	Bind();
	glTexImage2D(target, 0, internalFormat, width,
		height, 0, format, GL_UNSIGNED_BYTE, imageData);
}

Texture2D::Texture2D(GLenum textureUnit, GLuint id)
	: BaseTexture(GL_TEXTURE_2D, textureUnit, id), loaded(false)
{ }

Texture2D::Texture2D(const char *name, GLenum textureUnit, GLuint id)
	: BaseTexture(GL_TEXTURE_2D, textureUnit, id), loaded(false)
{
	LoadFromTGA(name);
}

bool Texture2D::LoadFromTGA(const char *name)
{
	Image img;
	loaded = loadFromTGA(name, img);
	if (loaded) {
		texImage2D(target, img.GetData());
	}
	return loaded;
}

void Texture2D::SetTexImage(GLenum level, GLint internalFormat, GLsizei width, GLsizei height,
	GLint border, GLenum format, GLenum type, const GLvoid *data)
{
	Bind();
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
}

CubeTexture::CubeTexture(GLenum textureUnit)
	: BaseTexture(GL_TEXTURE_CUBE_MAP, textureUnit), loaded(false)
{ }

CubeTexture::CubeTexture(const char **sides, GLenum textureUnit)
	: BaseTexture(GL_TEXTURE_CUBE_MAP, textureUnit), loaded(false)
{
	LoadFromTGA(sides);
}

bool CubeTexture::LoadFromTGA(const char **sides)
{
	SetFilters(GL_LINEAR, GL_LINEAR);
	SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	static GLenum targets[6] = {
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, // front
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // back
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // top
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // bottom
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // left
		GL_TEXTURE_CUBE_MAP_POSITIVE_X  // right
	};

	Image img;
	loaded = true;
	for (int i = 0; i < 6; i++) {
		loaded &= loadFromTGA(sides[i], img);
		if (!loaded) break;
		texImage2D(targets[i], img.GetData());
	}
	return loaded;
}