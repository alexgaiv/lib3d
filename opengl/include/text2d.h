#ifndef _TEXT_2D_
#define _TEXT_2D_

#include "vertexbuffer.h"
#include "texture.h"
#include "glcontext.h"
#include "sharedptr.h"

class Text2D;
class Font2D;

template<>
class shared_traits<Font2D>
{
public:
	struct Charset {
		int base;
		int startChar;
		int endChar;
	};

	int numChars, numCharsets;
	Charset *charsets;
	int *charWidth;

	Texture2D fontTexture;
	int numCellsX, numCellsY;
	float fontHeight;

	float dty;
	float numCellsX_inv, numCellsY_inv;
	float texWidth_inv;

	shared_traits();
	~shared_traits();
	bool load(const char *name);
};

class Font2D : public Shared<Font2D>
{
public:
	Font2D() : lineSpacing(0) { }
	Font2D(const char *filename);

	void LoadFnt(const char *filename);
	bool IsLoaded() { return loaded; }

	void SetColor(Color4f color) { this->color = color; }
	Color4f GetColor() const { return color; }
	int GetHeight() const { return (int)ptr->fontHeight; }
	void SetLineSpacing(int spacing) { lineSpacing = spacing; }
	int CalcTextWidth(const wchar_t *text);
private:
	friend class Text2D;
	friend class SharedTraits;

	Color4f color;
	int lineSpacing;
	bool loaded;
	
	SharedTraits *operator->() { return ptr.Get(); }
	const SharedTraits *operator->() const { return ptr.Get(); }
};

class Text2D
{
public:
	Font2D font;

	Text2D(GLRenderingContext *rc, const Font2D &font);
	Text2D(const Text2D &t);

	void SetText(const wchar_t *text);
	void Draw(int x, int y);
	
	Text2D &operator=(const Text2D &t);
private:
	GLRenderingContext *rc;
	int numVerts;
	VertexArrayObject vao;
	VertexBuffer vertices, texCoords;
	ProgramObject *prog;
	
	void drawFixed(int x, int y);
	void clone(const Text2D &t);
};

#endif // _TEXT_2D_