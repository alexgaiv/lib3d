#ifndef _GL_CONTEXT_H_
#define _GL_CONTEXT_H_

#include "common.h"
#include "datatypes.h"
#include "shader.h"
#include <stack>
#include <list>
#include <vector>

using namespace std;

class GLRenderingContext;
class _PO_Shared;
class ProgramObject;

class GLRC_Module
{
private:
	friend class GLRenderingContext;
	virtual const char *Name() = 0;
	virtual void Initialize(GLRenderingContext *rc) = 0;
	virtual void Destroy() = 0;
};

enum GLRenderingContextFlags
{
	GLRC_DEBUG = 1,
	GLRC_FORWARD_COMPATIBLE = 2,
	GLRC_CORE_PROFILE = 4,
	GLRC_COMPATIBILITY_PROFILE = 8,
	GLRC_REQUEST_API_VERSION = 16,
	GLRC_MSAA = 32
};

struct GLRenderingContextParams
{
	DWORD glrcFlags;
	int majorVersion;
	int minorVersion;
	int layerPlane;
	int msaaNumberOfSamples;
	PIXELFORMATDESCRIPTOR *pixelFormat; // can be NULL
};

class GLRenderingContext
{
public:
	HGLRC hrc;

	GLRenderingContext(HDC hdc, const GLRenderingContextParams *params = NULL);
	~GLRenderingContext();

	void MakeCurrent() { wglMakeCurrent(_hdc, hrc); }

	ProgramObject *GetCurProgram() { return curProgram; }

	void SetModelView(const Matrix44f &mat);
	void SetProjection(const Matrix44f &mat);
	void MultModelView(const Matrix44f &mat);
	void MultProjection(const Matrix44f &mat);

	Matrix44f GetModelView() const { return modelview; }
	Matrix44f GetProjection() const { return projection; }
	const Matrix44f &GetModelViewRef() const { return modelview; }
	const Matrix44f &GetProjectionRef() const { return projection; }

	void PushModelView()  { mvStack.push(modelview); }
	void PopModelView()   { SetModelView(mvStack.top()); mvStack.pop(); }
	void PushProjection() { projStack.push(projection); }
	void PopProjection()  { SetProjection(projStack.top()); projStack.pop(); }

	void AddModule(GLRC_Module *module);
	GLRC_Module *GetModule(const char *name);
private:
	friend class ProgramObject;
	friend class _PO_Shared;
	friend class BaseTexture;

	HDC _hdc;
	vector<GLRC_Module *> modules;
	list<_PO_Shared *> shaders;
	Matrix44f modelview, projection;
	stack<Matrix44f> mvStack, projStack;

	ProgramObject *curProgram;

	Matrix44f mvpMatrix, normalMatrix;
	bool mvpComputed;
	bool normComputed;

	void AttachProgram(_PO_Shared *prog) {
		shaders.push_back(prog);
	}
	void DetachProgram(_PO_Shared *prog) {
		shaders.remove(prog);
	}
	
	void set_mv(const Matrix44f &mat);
	void set_proj(const Matrix44f &mat);

	HGLRC createContextAttrib(HDC hdc, const GLRenderingContextParams *params);
};

#endif // _GL_CONTEXT_H_