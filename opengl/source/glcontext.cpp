#include "glcontext.h"
#include "glwindow.h"

GLRenderingContext::GLRenderingContext(HDC hdc,
	const GLRenderingContextParams *params) : hrc(NULL), _hdc(hdc)
{
	curProgram = NULL;
	mvpComputed = normComputed = false;

	PIXELFORMATDESCRIPTOR pfd = { };
	if (!params->pixelFormat)
	{
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
	}
	else pfd = *params->pixelFormat;

	if (!params || params->glrcFlags == 0)
	{
		int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, iPixelFormat, &pfd);
		hrc = wglCreateContext(hdc);
		return;
	}

	bool useMSAA = (params->glrcFlags & GLRC_MSAA) != 0;
	if (useMSAA)
	{
		GLWindow tmpWindow;
		tmpWindow.bDummy = true;
		tmpWindow.Create("");

		HDC hTempDC = GetDC(tmpWindow.m_hwnd);
		int iPixelFormat = ChoosePixelFormat(hTempDC, &pfd);
		SetPixelFormat(hTempDC, iPixelFormat, &pfd);

		HGLRC hTempRC = wglCreateContext(hTempDC);
		wglMakeCurrent(hTempDC, hTempRC);

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
			(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		useMSAA = wglChoosePixelFormatARB != NULL;
		if (useMSAA)
		{
			int attrs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB, pfd.cColorBits,
				WGL_DEPTH_BITS_ARB, pfd.cDepthBits,
				WGL_STENCIL_BITS_ARB, pfd.cStencilBits,
				WGL_SAMPLE_BUFFERS_ARB, 1,
				WGL_SAMPLES_ARB, params->msaaNumberOfSamples,
				0
			};

			int iPixelFormatMSAA = 0;
			UINT numFormats = 0;
			BOOL valid = wglChoosePixelFormatARB(hdc, attrs, NULL, 1, &iPixelFormatMSAA, &numFormats);
			if (valid && numFormats != 0)
			{
				SetPixelFormat(hdc, iPixelFormatMSAA, &pfd);
				if (params->glrcFlags != GLRC_MSAA)
					hrc = createContextAttrib(hdc, params);
				else hrc = wglCreateContext(hdc);
			}
			else useMSAA = false;
		}
		wglDeleteContext(hTempRC);
		ReleaseDC(tmpWindow.m_hwnd, hTempDC);
	}

	if (!useMSAA) {
		int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, iPixelFormat, &pfd);
		if (params->glrcFlags != GLRC_MSAA)
			hrc = createContextAttrib(hdc, params);
		else hrc = wglCreateContext(hdc);
	}

	this->MakeCurrent();
}

GLRenderingContext::~GLRenderingContext()
{
	if (hrc) {
		wglMakeCurrent(_hdc, NULL);
		wglDeleteContext(hrc);
	}
	for (int i = 0, s = modules.size(); i < s; i++) {
		modules[i]->Destroy();
		delete modules[i];
	}
}

HGLRC GLRenderingContext::createContextAttrib(HDC hdc, const GLRenderingContextParams *params)
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	if (!wglCreateContextAttribsARB) return wglCreateContext(hdc);

	int attribs[11] = { };
	int i = 0;

	if (params->glrcFlags & GLRC_REQUEST_API_VERSION)
	{
		attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attribs[i++] = params->majorVersion;
		attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attribs[i++] = params->minorVersion;
	}

	if (params->glrcFlags & (GLRC_DEBUG|GLRC_FORWARD_COMPATIBLE))
	{
		attribs[i++] = WGL_CONTEXT_FLAGS_ARB;
		if (params->glrcFlags & GLRC_DEBUG) {
			attribs[i++] = WGL_CONTEXT_DEBUG_BIT_ARB;
		}
		else if (params->glrcFlags & GLRC_FORWARD_COMPATIBLE) {
			attribs[i++] = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
	}

	if (params->glrcFlags & (GLRC_CORE_PROFILE|GLRC_COMPATIBILITY_PROFILE))
	{
		attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
		if (params->glrcFlags & GLRC_CORE_PROFILE) {
			attribs[i++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		}
		else if (params->glrcFlags & GLRC_COMPATIBILITY_PROFILE) {
			attribs[i++] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}
	}

	if (params->layerPlane != 0) {
		attribs[i++] = WGL_CONTEXT_LAYER_PLANE_ARB;
		attribs[i++] = params->layerPlane;
	}

	attribs[i] = 0;

	HGLRC hrc = wglCreateContextAttribsARB(hdc, NULL, attribs);
	return hrc ? hrc : wglCreateContext(hdc);
}


void GLRenderingContext::set_mv(const Matrix44f &mat)
{
	list<_PO_Shared *>::iterator pi;
	for (pi = shaders.begin(); pi != shaders.end(); pi++)
		(*pi)->fUpdateMV = true;
	mvpComputed = normComputed = false;
}

void GLRenderingContext::set_proj(const Matrix44f &mat)
{
	list<_PO_Shared *>::iterator pi;
	for (pi = shaders.begin(); pi != shaders.end(); pi++)
		(*pi)->fUpdateProj = true;
	mvpComputed = false;
}

void GLRenderingContext::SetModelView(const Matrix44f &mat)
{
	modelview = mat;
	if (curProgram) {
		set_mv(modelview);
	}
	else {
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(mat.data);
	}
}

void GLRenderingContext::SetProjection(const Matrix44f &mat)
{
	projection = mat;
	if (curProgram) {
		set_proj(projection);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(mat.data);
	}
}

void GLRenderingContext::MultModelView(const Matrix44f &mat)
{
	modelview *= mat;
	if (curProgram) {
		set_mv(modelview);
	}
	else {
		glMatrixMode(GL_MODELVIEW);
		glMultMatrixf(mat.data);
	}
}

void GLRenderingContext::MultProjection(const Matrix44f &mat)
{
	projection *= mat;
	if (curProgram) {
		set_proj(projection);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glMultMatrixf(mat.data);
	}
}

void GLRenderingContext::AddModule(GLRC_Module *module)
{
	if (GetModule(module->Name())) return;
	modules.push_back(module);
	module->Initialize(this);
}
GLRC_Module *GLRenderingContext::GetModule(const char *name)
{
	for (int i = 0, s = modules.size(); i < s; i++)
		if (!strcmp(name, modules[i]->Name())) return modules[i];
	return NULL;
}