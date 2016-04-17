#include "skybox.h"

class GLRC_SkyboxModule : public GLRC_Module
{
private:
	friend class Skybox;
	const char *Name() { return "Skybox"; }
	void Initialize(GLRenderingContext *rc);
	void Destroy();

	static float vertsData[108];
	static char *shaderSource[2];
	VertexBuffer *vertices;
	ProgramObject *prog;
};

void GLRC_SkyboxModule::Initialize(GLRenderingContext *rc)
{
	vertices = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	prog = new ProgramObject(rc);

	vertices->SetData(sizeof(vertsData), vertsData, GL_STATIC_DRAW);

	Shader vshader(GL_VERTEX_SHADER);
	Shader fshader(GL_FRAGMENT_SHADER);
	vshader.CompileSource(shaderSource[0]);
	fshader.CompileSource(shaderSource[1]);
	prog->AttachShader(vshader);
	prog->AttachShader(fshader);
	prog->Link();
	prog->Uniform("cubeTex", 0);
}

void GLRC_SkyboxModule::Destroy()
{
	delete vertices;
	delete prog;
}

float GLRC_SkyboxModule::vertsData[108] =
{
	-10.0f,  10.0f, -10.0f,
	-10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
  
	-10.0f, -10.0f,  10.0f,
	-10.0f, -10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f,  10.0f,
	-10.0f, -10.0f,  10.0f,
  
	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
   
	-10.0f, -10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f, -10.0f,  10.0f,
	-10.0f, -10.0f,  10.0f,
  
	-10.0f,  10.0f, -10.0f,
	10.0f,  10.0f, -10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f, -10.0f,
  
	-10.0f, -10.0f, -10.0f,
	-10.0f, -10.0f,  10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	-10.0f, -10.0f,  10.0f,
	10.0f, -10.0f,  10.0f
};

char *GLRC_SkyboxModule::shaderSource[2] =
{
	"#version 130\n"
	"uniform mat4 ModelView;"
	"uniform mat4 Projection;"
	"in vec3 Vertex;"
	"out vec3 fTexCoord;"
	"void main() {"
		"fTexCoord = Vertex;"
		"mat4 mv = ModelView;"
		"for (int i = 0; i < 3; i++)"
			"mv[i] = normalize(mv[i]);"
		"mv[3] = vec4(0, 0, 0, 1);"
		"gl_Position = Projection * mv * vec4(Vertex, 1.0);"
	"}"
	,
	"#version 130\n"
	"in vec3 fTexCoord;"
	"uniform samplerCube cubeTex;"
	"void main() {"
		"gl_FragColor = texture(cubeTex, fTexCoord);"
	"}"
};

Skybox::Skybox(GLRenderingContext *rc, const char **sides)
	: rc(rc), tex(sides)
{
	init();
}

Skybox::Skybox(GLRenderingContext *rc, const CubeTexture &tex) : rc(rc)
{
	this->tex = tex;
	init();
}

void Skybox::init()
{
	GLRC_SkyboxModule *module = (GLRC_SkyboxModule *)rc->GetModule("Skybox");
	if (!module) {
		module = new GLRC_SkyboxModule;
		rc->AddModule(module);
	}
	prog = module->prog;
	vertices = module->vertices;
}

void Skybox::Draw()
{
	glDepthMask(GL_FALSE);
	glEnableVertexAttribArray(AttribsLocations.Vertex);

	prog->Use();
	tex.Bind();
	vertices->Bind();
	vertices->AttribPointer(0, 3, GL_FLOAT);
	vertices->DrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDisableVertexAttribArray(AttribsLocations.Vertex);
}