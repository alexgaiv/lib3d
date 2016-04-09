#include "glwindow.h"
#include <strsafe.h>

GLWindow::GLWindow()
{
	m_hwnd = NULL;
	m_hdc = NULL;
	m_rc = NULL;
	bFullScreen = bVsyinc = bDummy = false;
}

HWND GLWindow::Create(LPCTSTR lpCaption, int x, int y, int width, int height,
	DWORD dwStyle, DWORD dwExStyle, HWND hParent)
{
	BaseWindow::Create(lpCaption, x, y, width, height,
		dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, dwExStyle, NULL, hParent);
	return m_hwnd;
}

void GLWindow::CreateFullScreen(LPCTSTR lpCaption)
{
	bFullScreen = true;

	RECT screenRect = { };
	GetClientRect(GetDesktopWindow(), &screenRect);

	this->changeDisplaySettings();
	this->Create(lpCaption, 0, 0, screenRect.right, screenRect.bottom, WS_POPUP, WS_EX_TOPMOST);

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(1);
		bVsyinc = true;
	}
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

HRESULT GLWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bDummy)
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	
	int ll = (int)(short)LOWORD(lParam);
	int hl = (int)(short)HIWORD(lParam);

	OnMessage(uMsg, wParam, lParam);
	switch(uMsg)
	{
	case WM_CREATE:
		initRC();
		OnCreate();
		return 0;
	case WM_PAINT:
		this->Redraw();
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