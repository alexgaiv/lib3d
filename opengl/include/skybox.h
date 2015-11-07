#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

class Skybox
{
public:
	Skybox(const char **sides, const char *vertShader, const char *fragShader);
	~Skybox();
	void Draw();
private:
	static float vertsData[];
	VertexBuffer vertices;
	ProgramObject prog;
	CubeTexture tex;
};

#endif // _SKYBOX_H_