#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "datatypes.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include <list>
#include <stack>

using namespace std;

class ProgramObject;

const struct {
	int Vertex;
	int Normal;
	int TexCoord;
	int Tangent;
	int Binormal;
} AttribsLocations = { 0, 1, 2, 3, 4 };

class Global
{
public:
	static void AttachProgram(ProgramObject *prog) {
		shaders.push_back(prog);
	}
	static void DetachProgram(ProgramObject *prog) {
		shaders.remove(prog);
	}

	static void SetModelView(const Matrix44f &mat);
	static void SetProjection(const Matrix44f &mat);
	static void MultModelView(const Matrix44f &mat);
	static void MultProjection(const Matrix44f &mat);
	static Matrix44f GetModelView() { return modelview; }
	static Matrix44f GetProjection() { return projection; }

	static void PushModelView()  { mvStack.push(modelview); }
	static void PopModelView()   { SetModelView(mvStack.top()); mvStack.pop(); }
	static void PushProjection() { projStack.push(projection); }
	static void PopProjection()  { SetProjection(projStack.top()); projStack.pop(); }
private:
	friend class ProgramObject;
	friend class Mesh;
	friend class BaseTexture;

	static GLuint curProgram;
	static GLenum curTextureUnit;

	static std::list<ProgramObject *> shaders;
	static Matrix44f modelview, projection;
	static stack<Matrix44f> mvStack, projStack;
	
	static void set_mv(const Matrix44f &mat);
	static void set_proj(const Matrix44f &mat);
};

#endif // _GLOBAL_H_