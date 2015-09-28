#include "d3dwindow.h"

#include <strsafe.h>

HWND D3DWindow::CreateParam(LPCTSTR lpCaption, int x, int y, int width, int height, DWORD dwStyle, DWORD dwExStyle)
{
	static ATOM reg = D3DWindow::_RegisterWindow(this);

	m_hwnd = ::CreateWindowEx(dwExStyle, MAKEINTATOM(reg), lpCaption,
		dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		x, y, width, height, NULL, NULL, GetModuleHandle(NULL), this);

	return m_hwnd;
}

HWND D3DWindow::CreateFullScreen(LPCTSTR lpCaption)
{
	bFullScreen = true;
	RECT screenRect = { };
	GetClientRect(GetDesktopWindow(), &screenRect);
	return CreateParam(lpCaption, 0, 0, screenRect.right, screenRect.bottom, WS_POPUP, WS_EX_APPWINDOW);
}

void D3DWindow::MainLoop()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

WindowInfoStruct D3DWindow::GetWindowInfo()
{
	TCHAR className[20] = TEXT("clsname:");
	StringCchPrintf(className+8, 10, TEXT("%d"), (int)this);

	WindowInfoStruct wi = { };
	wi.hCursor = LoadCursor(NULL, IDC_ARROW);
	wi.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wi.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wi.hbrBackground = NULL;
	wi.lpszMenuName =  NULL;
	wi.lpszClassName = className;

	return wi;
}

ATOM D3DWindow::_RegisterWindow(D3DWindow *pThis)
{
	WNDCLASSEX wc = { };
	WindowInfoStruct wi = pThis->GetWindowInfo();

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC;
	wc.lpfnWndProc = D3DWindow::_WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = wi.hIcon;
	wc.hCursor = wi.hCursor;
	wc.hbrBackground = wi.hbrBackground;
	wc.lpszMenuName = wi.lpszMenuName;
	wc.lpszClassName = wi.lpszClassName;
	wc.hIconSm = wi.hIconSm;
	
	return RegisterClassEx(&wc);
}

LRESULT CALLBACK D3DWindow::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	D3DWindow *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
		pThis = (D3DWindow *)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)pThis);

		pThis->m_hwnd = hwnd;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else {
		pThis = (D3DWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
		
	if (pThis) {
		return pThis->D3DWindow::_HandleMessage(uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT D3DWindow::_HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int ll = (short)LOWORD(lParam), hl = (short)HIWORD(lParam);
	OnMessage(uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_CREATE:
		_InitD3D();
		OnCreate();
		return 0;
	case WM_PAINT:
		OnDisplay();
		m_dev->Present(0, 0, 0, 0);
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
			}
			if (D3D_OK != m_dev->TestCooperativeLevel()) {
				bLost = true;
				OnDeviceLost();
			}
		} else {
			if (bFullScreen) SetFocus(m_hwnd);
			if (bLost && D3D_OK == m_dev->TestCooperativeLevel()) {
				bLost = false;
				OnDeviceActivated();
				m_dev->Reset(&_GetPresentParams());
			}
		}
		return 0;
	}
	case WM_DESTROY:
		OnDestroy();
		if (m_d3d) m_d3d->Release();
		if (m_dev) m_dev->Release();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

D3DPRESENT_PARAMETERS D3DWindow::_GetPresentParams()
{
	D3DPRESENT_PARAMETERS pp = { };
	D3DDISPLAYMODE mode = { };
	m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	pp.BackBufferWidth = bFullScreen ? mode.Width : 0;
	pp.BackBufferHeight = bFullScreen ? mode.Height : 0;
	pp.BackBufferFormat = mode.Format;
	pp.BackBufferCount = 1;
	pp.MultiSampleType = D3DMULTISAMPLE_NONE; 
	pp.MultiSampleQuality = 0; 
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.hDeviceWindow = m_hwnd; 
	pp.Windowed = !bFullScreen;
	pp.EnableAutoDepthStencil = TRUE;
	pp.AutoDepthStencilFormat = D3DFMT_D24S8;
	pp.Flags = 0;
	pp.FullScreen_RefreshRateInHz = bFullScreen ? mode.RefreshRate : 0;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	return pp;
}

bool D3DWindow::_InitD3D()
{
	if (NULL == (m_d3d = Direct3DCreate9(D3D_SDK_VERSION))) {
		MessageBox(m_hwnd, TEXT("Cannot create direct3d object"), TEXT("Error"), MB_ICONERROR);
		exit(1);
		return false;
	}

	D3DCAPS9 caps = { };
	m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	DWORD vp = caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ?
		D3DCREATE_HARDWARE_VERTEXPROCESSING :
		D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS pp = _GetPresentParams();
	if (FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		m_hwnd, vp, &pp, &m_dev)))
	{
		MessageBox(m_hwnd, TEXT("Cannot create direct3d device"), TEXT("Error"), MB_ICONERROR);
		exit(1);
		return false;
	}

	return true;
}