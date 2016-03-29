#include "glwindow.h"
#include <strsafe.h>

GLWindow::GLWindow()
{
	m_hwnd = NULL;
	m_hdc = NULL;
	m_rc = NULL;
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
	static ATOM reg = GLWindow::registerWindow(this);

	m_hwnd = CreateWindowEx(dwExStyle, MAKEINTATOM(reg), lpCaption,
		dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		x, y, width, height, hParent, NULL, GetModuleHandle(NULL), this);

	return m_hwnd;
}

void GLWindow::CreateFullScreen(LPCTSTR lpCaption)
{
	bFullScreen = true;

	RECT screenRect = { };
	GetClientRect(GetDesktopWindow(), &screenRect);

	this->changeDisplaySettings();
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

GLRenderingContextParams GLWindow::GetRCParams()
{
	GLRenderingContextParams params = { };
	params.glrcFlags = GLRC_COMPATIBILITY_PROFILE|GLRC_MSAA;
	params.msaaNumberOfSamples = 4;
	return params;
}

void GLWindow::initRC()
{
	m_hdc = GetDC(m_hwnd);
	m_rc = new GLRenderingContext(m_hdc, &GetRCParams());
}

void GLWindow::changeDisplaySettings()
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

ATOM GLWindow::registerWindow(GLWindow *pThis)
{
	WNDCLASSEX wc = { };
	GLWindowInfoStruct wi = pThis->GetWindowInfo();

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC;
	wc.lpfnWndProc = GLWindow::wndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = wi.hIcon;
	wc.hCursor = wi.hCursor;
	wc.hbrBackground = wi.hbrBackground;
	wc.lpszMenuName = wi.lpszMenuName;
	wc.lpszClassName = wi.lpszClassName;
	wc.hIconSm = wi.hIconSm;
	
	return RegisterClassEx(&wc);
}

LRESULT CALLBACK GLWindow::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		return pThis->GLWindow::handleMessage(uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT GLWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bDummy)
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);

	int ll = (short)LOWORD(lParam), hl = (short)HIWORD(lParam);
	OnMessage(uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_CREATE:
		initRC();
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
			if (bFullScreen) changeDisplaySettings();
		}
		return 0;
	}
	case WM_DESTROY:
		OnDestroy();
		delete m_rc;
		ReleaseDC(m_hwnd, m_hdc);
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}