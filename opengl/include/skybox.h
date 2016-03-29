#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "glcontext.h"
#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

class Skybox
{
public:
	Skybox(GLRenderingContext *rc, const char **sides);
	Skybox(GLRenderingContext *rc, const CubeTexture &tex);

	CubeTexture GetTexture() { return tex; }
	void Draw();
private:
	GLRenderingContext *rc;
	VertexBuffer *vertices;
	ProgramObject *prog;
	CubeTexture tex;

	void init();
};

#endif // _SKYBOX_H_