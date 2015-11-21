#include "glwindow.h"
#include <strsafe.h>

GLWindow::GLWindow()
{
	m_hwnd = NULL;
	m_hdc = NULL;
	m_hrc = NULL;
	bFullScreen = bDummy = false;
}

GLWindow::~GLWindow()
{
	if (IsWindow(m_hwnd)) {
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
		DestroyWindow(m_hwnd);
	}
}

HWND GLWindow::CreateParam(LPCTSTR lpCaption, int x, int y, int width, int height,
	DWORD dwStyle, DWORD dwExStyle, HWND hParent)
{
	static ATOM reg = GLWindow::_RegisterWindow(this);

	m_hwnd = ::CreateWindowEx(dwExStyle, MAKEINTATOM(reg), lpCaption,
		dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		x, y, width, height, hParent, NULL, GetModuleHandle(NULL), this);

	return m_hwnd;
}

void GLWindow::CreateFullScreen(LPCTSTR lpCaption)
{
	bFullScreen = true;

	RECT screenRect = { };
	GetClientRect(GetDesktopWindow(), &screenRect);

	this->_ChangeDisplaySettings();
	this->CreateParam(lpCaption, 0, 0, screenRect.right, screenRect.bottom, WS_POPUP, WS_EX_TOPMOST);

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT) wglSwapIntervalEXT(1);
}

void GLWindow::MainLoop()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

GLWindowInfoStruct GLWindow::GetWindowInfo()
{
	TCHAR className[20] = TEXT("clsname:");
	StringCchPrintf(className+8, 10, TEXT("%d"), (int)this);

	GLWindowInfoStruct wi = { };
	wi.hCursor = LoadCursor(NULL, IDC_ARROW);
	wi.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wi.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wi.hbrBackground = NULL;
	wi.lpszMenuName =  NULL;
	wi.lpszClassName = className;

	return wi;
}

PIXELFORMATDESCRIPTOR GLWindow::GetDCPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	return pfd;
}

bool GLWindow::_CreateCompabilityContext()
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	if (wglCreateContextAttribsARB != NULL)
	{
		int attribs[] = {
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		m_hrc = wglCreateContextAttribsARB(m_hdc, NULL, attribs);
		if (m_hrc) return true;
	}
	return false;
}

void GLWindow::_InitRC()
{
	m_hdc = GetDC(m_hwnd);
	PIXELFORMATDESCRIPTOR pfd = this->GetDCPixelFormat();

	GLWindow tmpWindow;
	tmpWindow.bDummy = true;
	tmpWindow.CreateParam("");

	HDC hTempDC = GetDC(tmpWindow.m_hwnd);
	int iPixelFormat = ChoosePixelFormat(hTempDC, &pfd);
	SetPixelFormat(hTempDC, iPixelFormat, &pfd);

	HGLRC hTempRC = wglCreateContext(hTempDC);
	wglMakeCurrent(hTempDC, hTempRC);

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	bool useMSAA = wglChoosePixelFormatARB != NULL;
	if (useMSAA)
	{
		int attrs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB, 4,
			0
		};

		int iPixelFormatMSAA = 0;
		UINT numFormats = 0;
		BOOL valid = wglChoosePixelFormatARB(m_hdc, attrs, NULL, 1, &iPixelFormatMSAA, &numFormats);
		if (valid && numFormats != 0)
		{
			SetPixelFormat(m_hdc, iPixelFormatMSAA, &pfd);
			if (!_CreateCompabilityContext()) {
				m_hrc = wglCreateContext(m_hdc);
			}
		}
		else useMSAA = false;
	}

	if (!useMSAA)
	{
		int iPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
		SetPixelFormat(m_hdc, iPixelFormat, &pfd);

		if (!_CreateCompabilityContext()) {
			m_hrc = wglCreateContext(m_hdc);
		}
	}

	wglMakeCurrent(m_hdc, m_hrc);
	wglDeleteContext(hTempRC);
	ReleaseDC(tmpWindow.m_hwnd, hTempDC);
}

void GLWindow::_ChangeDisplaySettings()
{
	RECT screenRect = { };
	GetClientRect(GetDesktopWindow(), &screenRect);

	DEVMODE deviceMode = { };
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceMode);
	deviceMode.dmSize = sizeof(DEVMODE);
	deviceMode.dmPelsWidth = screenRect.right;
	deviceMode.dmPelsHeight = screenRect.bottom;
	ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
}

ATOM GLWindow::_RegisterWindow(GLWindow *pThis)
{
	WNDCLASSEX wc = { };
	GLWindowInfoStruct wi = pThis->GetWindowInfo();

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC;
	wc.lpfnWndProc = GLWindow::_WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = wi.hIcon;
	wc.hCursor = wi.hCursor;
	wc.hbrBackground = wi.hbrBackground;
	wc.lpszMenuName = wi.lpszMenuName;
	wc.lpszClassName = wi.lpszClassName;
	wc.hIconSm = wi.hIconSm;
	
	return RegisterClassEx(&wc);
}

LRESULT CALLBACK GLWindow::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GLWindow *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
		pThis = (GLWindow *)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)pThis);

		pThis->m_hwnd = hwnd;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else {
		pThis = (GLWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
		
	if (pThis) {
		return pThis->GLWindow::_HandleMessage(uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT GLWindow::_HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bDummy)
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);

	int ll = (short)LOWORD(lParam), hl = (short)HIWORD(lParam);
	OnMessage(uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_CREATE:
		_InitRC();
		OnCreate();
		return 0;
	case WM_PAINT:
		OnDisplay();
		SwapBuffers(m_hdc);
		ValidateRect(m_hwnd, NULL);
		return 0;
	case WM_SIZE:
		OnSize(ll, hl);
		return 0;
	case WM_LBUTTONDOWN:
		OnMouseDown(MouseButton::LBUTTON, ll, hl);
		SetCapture(m_hwnd);
		return 0;
	case WM_MBUTTONDOWN:
		OnMouseDown(MouseButton::MBUTTON, ll, hl);
		SetCapture(m_hwnd);
		return 0;
	case WM_RBUTTONDOWN:
		OnMouseDown(MouseButton::RBUTTON, ll, hl);
		SetCapture(m_hwnd);
		return 0;
	case WM_LBUTTONUP:
		OnMouseUp(MouseButton::LBUTTON, ll, hl);
		ReleaseCapture();
		return 0;
	case WM_MBUTTONUP:
		OnMouseUp(MouseButton::MBUTTON, ll, hl);
		ReleaseCapture();
		return 0;
	case WM_RBUTTONUP:
		OnMouseUp(MouseButton::RBUTTON, ll, hl);
		ReleaseCapture();
		return 0;
	case WM_LBUTTONDBLCLK:
		OnMouseDblClick(MouseButton::LBUTTON, ll, hl);
		return 0;
	case WM_MBUTTONDBLCLK:
		OnMouseDblClick(MouseButton::MBUTTON, ll, hl);
		return 0;
	case WM_RBUTTONDBLCLK:
		OnMouseDblClick(MouseButton::RBUTTON, ll, hl);
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, ll, hl);
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel(HIWORD(wParam), LOWORD(wParam), ll, hl);
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		OnKeyUp(wParam);
		return 0;
	case WM_CHAR:
		OnChar(wParam);
		return 0;
	case WM_TIMER:
		OnTimer();
		return 0;
	case WM_ACTIVATE:
	{
		WORD active = LOWORD(wParam);
		if (active == WA_INACTIVE) {
			if (bFullScreen) {
				ShowWindow(m_hwnd, SW_SHOWMINIMIZED);
				ChangeDisplaySettings(NULL, 0);
			}
		} else {
			SetFocus(m_hwnd);
			if (bFullScreen) _ChangeDisplaySettings();
		}
		return 0;
	}
	case WM_DESTROY:
		OnDestroy();
		wglMakeCurrent(m_hdc, NULL);
		wglDeleteContext(m_hrc);
		ReleaseDC(m_hwnd, m_hdc);
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}