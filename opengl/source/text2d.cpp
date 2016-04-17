#include "text2d.h"
#include "datatypes.h"
#include "transform.h"
#include "glcontext.h"

Font2D::Font2D(const char *name) : ptr(new Shared), lineSpacing(0)
{
	ptr->load(name);
}

Font2D::Shared::Shared()
{
	numChars = numCharsets = 0;
	charsets = NULL;
	charWidth = NULL;
	numCellsX = numCellsY = 0;
	fontHeight = 0.0f;

	dty = 0.0f;
	numCellsX_inv = numCellsY_inv = 0.0f;
	texWidth_inv = 0.0f;
}

Font2D::Shared::~Shared()
{
	delete [] charsets;
	delete [] charWidth;
}

void Font2D::Shared::load(const char *filename)
{
	ifstream file(filename);
	if (!file) return;

	int curCharset = 0;
	int cellWidth = 0, cellHeight = 0;

	while (!file.eof())
	{
		char prefix[3] = { };
		file.read(prefix, 2);
		
		if (!strcmp(prefix, "nc"))
			file >> numChars;
		else if (!strcmp(prefix, "ns")) {
			file >> numCharsets;
			charsets = new Charset[numCharsets];
			memset(charsets, 0, numCharsets*sizeof(Charset));
		}
		else if (!strcmp(prefix, "f "))
		{
			char path[MAX_PATH] = { };
			file >> path;
			fontTexture.LoadFromTGA(path);
			fontTexture.SetFilters(GL_LINEAR, GL_LINEAR);
		}
		else if (!strcmp(prefix, "cw")) {
			file >> cellWidth;
		}
		else if (!strcmp(prefix, "ch")) {
			file >> cellHeight;
		}
		else if (!strcmp(prefix, "fh")) {
			file >> fontHeight;
		}
		else if (!strcmp(prefix, "fw"))
		{
			charWidth = new int[numChars];
			for (int i = 0; i < numChars; i++)
				file >> charWidth[i];
		}
		else if (!strcmp(prefix, "cs")) {
			if (curCharset < numCharsets) {
				Charset charset = { };
				file >> charset.base >> charset.startChar >> charset.endChar;
				charsets[curCharset++] = charset;
			}
		}
		ws(file);
	}

	if (!fontTexture.IsLoaded()) return;

	int tw = fontTexture.GetWidth();
	int th = fontTexture.GetHeight();

	dty = fontHeight / th;
	texWidth_inv = 1.0f / tw;

	if (cellWidth != 0) {
		numCellsX = tw / cellWidth;
		numCellsX_inv = 1.0f / numCellsX;
	}
	if (cellHeight != 0) {
		numCellsY = th / cellHeight;
		numCellsY_inv = 1.0f / numCellsY;
	}
}

int Font2D::CalcTextWidth(const wchar_t *text)
{
	int w = 0;
	for (int i = 0; *text; i++, text++)
	{
		wchar_t ch = *text;
		int index = 0;

		bool found = false;
		for (int j = 0; j < ptr->numCharsets; j++)
		{
			Font2D::Charset &c = ptr->charsets[j];
			if (ch >= c.startChar && ch <= c.endChar)
			{
				index = ch - c.startChar + c.base;
				found = true;
				break;
			}
		}

		if (!found) continue;
		w += ptr->charWidth[index];
	}
	return w;
}

class GLRC_Text2DModule : public GLRC_Module
{
private:
	friend class Text2D;
	const char *Name() { return "Text2D"; }
	void Initialize(GLRenderingContext *rc);
	void Destroy();

	static const char *shaderSource[2];
	ProgramObject *prog;
};

const char *GLRC_Text2DModule::shaderSource[2] = 
{
	"attribute vec3 Vertex;"
	"attribute vec2 TexCoord;"
	"varying vec2 fTexCoord;"
	"uniform mat4 ModelViewProjection;"
	"void main() {"
		"fTexCoord = TexCoord;"
		"gl_Position = ModelViewProjection * vec4(Vertex, 1.0);"
	"}"
	,
	"varying vec2 fTexCoord;"
	"uniform sampler2D ColorMap;"
	"uniform vec4 Color;"
	"void main() {"
		"gl_FragColor = vec4(Color.xyz, Color.a * texture2D(ColorMap, fTexCoord).a);"
	"}"
};

void GLRC_Text2DModule::Initialize(GLRenderingContext *rc)
{
	if (GLEW_ARB_shader_objects) {
		prog = new ProgramObject(rc);

		Shader vshader(GL_VERTEX_SHADER);
		Shader fshader(GL_FRAGMENT_SHADER);
		vshader.CompileSource(shaderSource[0]);
		fshader.CompileSource(shaderSource[1]);
		prog->AttachShader(vshader);
		prog->AttachShader(fshader);
		prog->Link();
		prog->Uniform("ColorMap", 0);
	}
	else prog = NULL;
}

void GLRC_Text2DModule::Destroy() {
	delete prog;
}

Text2D::Text2D(GLRenderingContext *rc, const Font2D &font) :
	rc(rc),
	font(font),
	vertices(rc, GL_ARRAY_BUFFER),
	texCoords(rc, GL_ARRAY_BUFFER),
	numVerts(0)
{
	GLRC_Text2DModule *module = (GLRC_Text2DModule *)rc->GetModule("Text2D");
	if (!module) {
		module = new GLRC_Text2DModule;
		rc->AddModule(module);
	}
	prog = module->prog;
}

Text2D::Text2D(const Text2D &t) :
	font(t.font),
	vertices(t.rc, GL_ARRAY_BUFFER),
	texCoords(t.rc, GL_ARRAY_BUFFER)
{
	clone(t);
}

Text2D &Text2D::operator=(const Text2D &t)
{
	font = t.font;
	clone(t);
	return *this;
}

void Text2D::clone(const Text2D &t)
{
	rc = t.rc;
	prog = t.prog;
	numVerts = t.numVerts;
	if (numVerts != 0) {
		t.vertices.CloneTo(vertices);
		t.texCoords.CloneTo(texCoords);
	}
}

void Text2D::SetText(const wchar_t *text)
{
	numVerts = 6*wcslen(text);
	Vector3f *verts = new Vector3f[numVerts];
	Vector2f *texs = new Vector2f[numVerts];

	float w = 0, h = 0;
	for (int i = 0; *text; i++, text++)
	{
		wchar_t ch = *text;
		int index = 0;
		int x = 0, y = 0;

		if (ch == '\n')
		{
			h += font->fontHeight + font.lineSpacing;
			w = 0;
			continue;
		}

		bool found = false;
		for (int j = 0; j < font->numCharsets; j++)
		{
			Font2D::Charset &c = font->charsets[j];
			if (ch >= c.startChar && ch <= c.endChar)
			{
				index = ch - c.startChar + c.base;
				x = index % font->numCellsX;
				y = index / font->numCellsY;
				found = true;
				break;
			}
		}

		if (!found) continue;

		float dw = (float)font->charWidth[index];
		int k = 6*i;
		verts[k]   = Vector3f(w, h, 0);
		verts[k+1] = Vector3f(w, h + font->fontHeight, 0);
		verts[k+2] = Vector3f(w + dw, h, 0);
		verts[k+3] = verts[k+2];
		verts[k+4] = verts[k+1];
		verts[k+5] = Vector3f(w + dw, h + font->fontHeight, 0);

		float tx = (float)x * font->numCellsX_inv;
		float ty = (float)y * font->numCellsY_inv;
		float dtx = dw * font->texWidth_inv;
		
		texs[k]   = Vector2f(tx, ty);
		texs[k+1] = Vector2f(tx, ty + font->dty);
		texs[k+2] = Vector2f(tx + dtx, ty);
		texs[k+3] = texs[k+2];
		texs[k+4] = texs[k+1];
		texs[k+5] = Vector2f(tx + dtx, ty + font->dty);

		w += dw;
	}
	
	vertices.SetData(numVerts*sizeof(Vector3f), verts, GL_STATIC_DRAW);
	texCoords.SetData(numVerts*sizeof(Vector2f), texs, GL_STATIC_DRAW);

	delete [] verts;
	delete [] texs;
}

void Text2D::drawFixed(int x, int y)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4fv(font.color.data);

	vertices.VertexPointer(3, GL_FLOAT, 0);
	texCoords.TexCoordPointer(2, GL_FLOAT, 0);
	vertices.DrawArrays(GL_TRIANGLES, 0, numVerts);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Text2D::Draw(int x, int y)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	font->fontTexture.Bind();

	rc->PushProjection();
	rc->PushModelView();

	float viewport[4] = { };
	glGetFloatv(GL_VIEWPORT, viewport);
	rc->SetProjection(Ortho2D(0, viewport[2], viewport[3], 0));
	rc->SetModelView(Translate((float)x, (float)y, 0));
	
	if (!GLEW_ARB_shader_objects)
		drawFixed(x, y);
	else
	{
		glEnableVertexAttribArray(AttribsLocations.Vertex);
		glEnableVertexAttribArray(AttribsLocations.TexCoord);

		prog->Use();
		prog->Uniform("Color", 1, font.color.data);

		vertices.AttribPointer(AttribsLocations.Vertex, 3, GL_FLOAT);
		texCoords.AttribPointer(AttribsLocations.TexCoord, 2, GL_FLOAT);
		vertices.DrawArrays(GL_TRIANGLES, 0, numVerts);

		glDisableVertexAttribArray(AttribsLocations.Vertex);
		glDisableVertexAttribArray(AttribsLocations.TexCoord);
	}
	
	glDisable(GL_BLEND);
	rc->PopModelView();
	rc->PopProjection();
}