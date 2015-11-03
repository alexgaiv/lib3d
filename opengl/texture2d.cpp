#include "texture2d.h"
#include <new>

Texture2D::Texture2D()
	: id(0), imageData(NULL), _loaded(false), _internalFormat(0), _format(0)
{
	ZeroMemory(&tgaHeader, sizeof(TGAHEADER));
}

Texture2D::Texture2D(LPCWSTR filename, UINT id)
	: imageData(NULL), _loaded(false), _internalFormat(0), _format(0)
{
	this->id = id;
	ZeroMemory(&tgaHeader, sizeof(TGAHEADER));
	LoadFromTGA(filename);
}

Texture2D::~Texture2D()
{
	if (imageData) delete [] imageData;
}

void Texture2D::SetAsTexImage() {
	if (!_loaded) return;
	this->Bind();

	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, tgaHeader.width,
		tgaHeader.height, 0, _format, GL_UNSIGNED_BYTE, imageData);
}

void Texture2D::BuildMipmaps() {
	if (!_loaded) return;
	this->Bind();

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tgaHeader.width, tgaHeader.height,
		_format, GL_UNSIGNED_BYTE, imageData);
}

void Texture2D::SetMinFilter(GLint filter) {
	this->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture2D::SetMagFilter(GLint filter) {
	this->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

bool Texture2D::_read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes)
{
	DWORD bytesRead;
	BOOL success = ReadFile(hFile, lpBuffer, nNumBytes, &bytesRead, NULL);

	if (!success || bytesRead != nNumBytes) {
		CloseHandle(hFile);
		_loaded = false;
		return false;
	}

	return true;
}

bool Texture2D::LoadFromTGA(LPCWSTR filename)
{
	HANDLE file = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (file == INVALID_HANDLE_VALUE) {
		_loaded = false;
		return false;
	}

	if (!_read(file, &tgaHeader, sizeof(TGAHEADER))) return false;

	if (tgaHeader.colorMapType != 0 ||
		tgaHeader.imageType != 2 && tgaHeader.imageType != 3)
	{
		CloseHandle(file);
		_loaded = false;
		return false;
	}

	switch (tgaHeader.depth)
	{
	case 8:
		_internalFormat = GL_LUMINANCE8;
		_format = GL_LUMINANCE;
		break;
	case 24:
		_internalFormat = GL_RGB8;
		_format = GL_BGR_EXT;
		break;
	case 32:
		_internalFormat = GL_RGBA8;
		_format = GL_BGRA_EXT;
		break;
	default:
		CloseHandle(file);
		_loaded = false;
		return false;
	}

	if (tgaHeader.idLength != 0) {
		SetFilePointer(file, tgaHeader.idLength, NULL, FILE_CURRENT);
	}

	DWORD imageSize = tgaHeader.width * tgaHeader.height * tgaHeader.depth / 8;
	if (imageData) delete [] imageData;
	imageData = new(std::nothrow) BYTE[imageSize];

	if (!imageData) {
		CloseHandle(file);
		_loaded = false;
		return false;
	}

	if (!_read(file, imageData, imageSize)) return false;

	CloseHandle(file);
	_loaded = true;
	return true;
}