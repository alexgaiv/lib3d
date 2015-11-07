#include "texture.h"
#include <new>

#pragma pack(push, 1)
struct TGAHEADER
{
	BYTE   idLength;
	BYTE   colorMapType;
	BYTE   imageType;
	USHORT colorMapOffset;
	USHORT colorMapLength;
	BYTE   colorMapEntrySize;
	USHORT xOrigin;
	USHORT yOrigin;
	USHORT width;
	USHORT height;
	BYTE   depth;
	BYTE   descriptor;
};
#pragma pack(pop)

BaseTexture::BaseTexture(GLenum target, GLuint id)
{
	this->target = target;
	width = height = 0;
	internalFormat = format = 0;

	if (id == GLuint(-1))
		glGenTextures(1, &this->id);
	else {
		this->id = id;
	}
}

void BaseTexture::SetFilters(GLint minFilter, GLint magFilter)
{
	Bind();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter == -1 ? minFilter : magFilter);
}

void BaseTexture::SetWrapMode(GLint wrapS, GLint wrapT, GLint wrapR)
{
	Bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	if (wrapR != -1)
		glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
}

void BaseTexture::_read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes)
{
	DWORD bytesRead;
	BOOL success = ReadFile(hFile, lpBuffer, nNumBytes, &bytesRead, NULL);
	if (!success || bytesRead != nNumBytes)
		throw false;
}

bool BaseTexture::_loadFromTGA(const char *filename, BYTE *&data)
{
	HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	data = 0;
	try {
		TGAHEADER tgaHeader = { };
		_read(file, &tgaHeader, sizeof(TGAHEADER));

		if (tgaHeader.colorMapType != 0 ||
			tgaHeader.imageType != 2 && tgaHeader.imageType != 3) {
			throw false;
		}

		switch (tgaHeader.depth)
		{
		case 8:
			internalFormat = 1;
			format = GL_LUMINANCE;
			break;
		case 24:
			internalFormat = 3;
			format = GL_BGR;
			break;
		case 32:
			internalFormat = 4;
			format = GL_BGRA;
			break;
		default:
			throw false;
		}

		if (tgaHeader.idLength != 0) {
			SetFilePointer(file, tgaHeader.idLength, NULL, FILE_CURRENT);
		}

		DWORD imageSize = tgaHeader.width * tgaHeader.height * tgaHeader.depth / 8;
		data = new(std::nothrow) BYTE[imageSize];
		if (!data) throw false;

		_read(file, data, imageSize);
		width = tgaHeader.width;
		height = tgaHeader.height;

		int rowSize = imageSize / height;
		if ((tgaHeader.descriptor & 0x10) == 0) {
			BYTE *tmp = new BYTE[rowSize];
			for (int i = 0; i < height / 2; i++)
			{
				BYTE *r1 = &data[rowSize*i];
				BYTE *r2 = &data[rowSize*(height - i - 1)];
				memcpy(tmp, r1, rowSize);
				memcpy(r1, r2, rowSize);
				memcpy(r2, tmp, rowSize);
			}
			delete [] tmp;
		}

		if ((tgaHeader.descriptor & 8) == 1) {
			RGBTRIPLE tmp;
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width / 2; j++) {
					RGBTRIPLE *p1 = (RGBTRIPLE *)&data[rowSize*i+j*3];
					RGBTRIPLE *p2 = (RGBTRIPLE *)&data[rowSize*i+(rowSize - (j+1)*3)];
					tmp = *p1;
					*p1 = *p2;
					*p2 = tmp;
				}
			}
		}
	}
	catch(bool) {
		if (data) {
			delete [] data;
			data = 0;
		}
		CloseHandle(file);
		return false;
	}
	CloseHandle(file);
	return true;
}

void BaseTexture::_texImage2D(GLenum target, BYTE *imageData)
{
	Bind();
	glTexImage2D(target, 0, internalFormat, width,
		height, 0, format, GL_UNSIGNED_BYTE, imageData);
}

Texture2D::Texture2D(GLuint id)
	: BaseTexture(GL_TEXTURE_2D, id), loaded(false)
{ }

Texture2D::Texture2D(const char *filename, GLuint id)
	: BaseTexture(GL_TEXTURE_2D, id), loaded(false)
{
	LoadFromTGA(filename);
}

bool Texture2D::LoadFromTGA(const char *filename)
{
	BYTE *imageData = NULL;
	loaded = _loadFromTGA(filename, imageData);
	if (loaded) {
		_texImage2D(target, imageData);
		delete [] imageData;
	}
	return loaded;
}

void Texture2D::BuildMipmaps() {
	if (!loaded) return;
	Bind();

	BYTE *imageData = new BYTE[width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_UNSIGNED_BYTE, imageData);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
		internalFormat, GL_UNSIGNED_BYTE, imageData);
	delete [] imageData;
}

CubeTexture::CubeTexture()
	: BaseTexture(GL_TEXTURE_CUBE_MAP), loaded(false)
{ }

CubeTexture::CubeTexture(const char **sides)
	: BaseTexture(GL_TEXTURE_CUBE_MAP), loaded(false)
{
	LoadFromTGA(sides);
}

bool CubeTexture::LoadFromTGA(const char **sides)
{
	SetFilters(GL_LINEAR);
	SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	static GLenum targets[6] = {
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, // front
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // back
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // top
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // bottom
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // left
		GL_TEXTURE_CUBE_MAP_POSITIVE_X  // right
	};

	loaded = true;
	for (int i = 0; i < 6; i++) {
		BYTE *imageData = NULL;
		loaded &= _loadFromTGA(sides[i], imageData);
		if (!loaded) break;
		_texImage2D(targets[i], imageData);
		delete [] imageData;
	}
	return loaded;
}