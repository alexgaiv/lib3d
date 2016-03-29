#include "image.h"

Image Image::Clone()
{
	Image img;
	img.isGood = isGood;
	img.width = width;
	img.height = height;
	img.dataSize = dataSize;

	if (dataSize != 0) {
		img.ptr->data = new BYTE[dataSize];
		memcpy(img.ptr->data, ptr->data, dataSize);
	}
	return img;
}

void Image::read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes)
{
	DWORD bytesRead;
	BOOL success = ReadFile(hFile, lpBuffer, nNumBytes, &bytesRead, NULL);
	if (!success || bytesRead != nNumBytes)
		throw false;
}

bool Image::LoadTga(const char *filename)
{
	ptr = my_shared_ptr<Shared>(new Shared);
	width = height = depth = 0;
	dataSize = 0;

	HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (file == INVALID_HANDLE_VALUE)
		return isGood = false;

	BYTE *data = 0;
	isGood = true;
	try {
		TGAHEADER tgaHeader = { };
		read(file, &tgaHeader, sizeof(TGAHEADER));

		if (tgaHeader.colorMapType != 0 ||
			tgaHeader.imageType != 2 && tgaHeader.imageType != 3) {
			throw false;
		}

		if (tgaHeader.idLength != 0) {
			SetFilePointer(file, tgaHeader.idLength, NULL, FILE_CURRENT);
		}

		DWORD imageSize = tgaHeader.width * tgaHeader.height * tgaHeader.depth / 8;
		data = new(std::nothrow) BYTE[imageSize];
		if (!data) throw false;

		read(file, data, imageSize);
		width = tgaHeader.width;
		height = tgaHeader.height;

		int rowSize = imageSize / height;
		if ((~tgaHeader.descriptor & 0x10)) {
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

		if (tgaHeader.descriptor & 8) {
			int components = tgaHeader.depth / 8;
			BYTE *tmp = new BYTE[components];
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width / 2; j++) {
					BYTE *c1 = &data[rowSize*i + j*components];
					BYTE *c2 = &data[rowSize*i + (rowSize - (j+1)*components)];
					memcpy(tmp, c1, components);
					memcpy(c1, c2, components);
					memcpy(c2, tmp, components);
				}
			}
			delete [] tmp;
		}
		
		ptr->data = data;
		dataSize = imageSize;
		depth = tgaHeader.depth;
	}
	catch(bool) {
		delete [] data;
		isGood = false;
	}

	CloseHandle(file);
	return isGood;
}