#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

class Skybox
{
public:
	Skybox(const char **sides);
	Skybox(const CubeTexture &tex);
	~Skybox();

	CubeTexture GetTexture() { return tex; }
	void Draw();
private:
	static float vertsData[];
	static char *source[2];
	VertexBuffer vertices;
	ProgramObject prog;
	CubeTexture tex;

	void _init();
};

#endif // _SKYBOX_H_