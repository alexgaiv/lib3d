#include "skybox.h"
#include "global.h"

float Skybox::vertsData[] =
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

char *Skybox::source[2] =
{
	"uniform mat4 ModelView;"
	"uniform mat4 Projection;"
	"attribute vec3 Vertex;"
	"varying vec3 fTexCoord;"
	"void main() {"
		"fTexCoord = Vertex;"
		"mat4 mv = ModelView;"
		"for (int i = 0; i < 3; i++)"
			"mv[i] = normalize(mv[i]);"
		"mv[3] = vec4(0, 0, 0, 1);"
		"gl_Position = Projection * mv * vec4(Vertex, 1.0);"
	"}",
	"varying vec3 fTexCoord;"
	"uniform samplerCube cubeTex;"
	"void main() {"
		"gl_FragColor = texture(cubeTex, fTexCoord);"
	"}"
};

Skybox::Skybox(const char **sides) : tex(sides, GL_TEXTURE0) {
	_init();
}

Skybox::Skybox(const CubeTexture &tex) {
	this->tex = tex;
	this->tex.SetTextureUnit(GL_TEXTURE0);
	_init();
}

void Skybox::_init()
{
	vertices.SetData(sizeof(vertsData), vertsData, GL_STATIC_DRAW);

	Shader vshader(GL_VERTEX_SHADER);
	Shader fshader(GL_FRAGMENT_SHADER);
	vshader.CompileSource(source[0]);
	fshader.CompileSource(source[1]);
	prog.AttachShader(vshader);
	prog.AttachShader(fshader);
	prog.Link();

	if (prog.IsLinked()) {
		prog.Uniform("cubeTex", 0);
	}
}

Skybox::~Skybox() {
	tex.Delete();
}

void Skybox::Draw()
{
	glDepthMask(GL_FALSE);
	glEnableVertexAttribArray(0);

	prog.Use();
	tex.Bind();
	vertices.Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDisableVertexAttribArray(0);
}