#ifndef _IAMGE_H_
#define _IAMGE_H_

#include "common.h"
#include "sharedptr.h"
#include "datatypes.h"
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

class Image
{
private:
	struct Shared;
	my_shared_ptr<Shared> ptr;
public:
	Image() : isGood(false), dataSize(0), ptr(new Shared) {
		width = height = depth = 0;
	}
	bool LoadTga(const char *filename);

	operator bool() const { return isGood; }
	bool IsGood() const { return isGood; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetDepth() const { return depth; }
	int GetDataSize() const { return dataSize; }
	BYTE *GetData() const { return ptr->data; };

	Color4b GetPixel(int x, int y) const
	{
		int i = x + y * width;
		switch (depth) {
		case 8:
			return Color4b(ptr->data[i]);
		case 24:
			return ((Color3b *)ptr->data)[i];
		case 32:
			return ((Color4b *)ptr->data)[i];
		default:
			return Color4b();
		}
	}

	Image Clone();
private:
	struct Shared
	{
		BYTE *data;
		Shared() : data(0) { }
		~Shared() { delete [] data; }
	};

	bool isGood;
	int dataSize;
	int width, height;
	int depth;
	void read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes);
};

#endif // _IAMGE_H_