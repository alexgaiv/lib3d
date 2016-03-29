#include "BaseWindow.h"

BaseWindow::~BaseWindow()
{
	if (IsWindow(m_hwnd)) {
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
		DestroyWindow(m_hwnd);
	}
}

BOOL BaseWindow::Create(LPCTSTR lpWindowName, int x, int y, int width, int height,
	DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HWND hWndParent)
{
	static ATOM reg = this->_RegisterWindow(this);

	m_hwnd = CreateWindowEx(dwExStyle, MAKEINTATOM(reg), lpWindowName, dwStyle,
		x, y, width, height, hWndParent, hMenu, GetModuleHandle(NULL), this);
	return m_hwnd ? TRUE : FALSE;
}

void BaseWindow::SetChildrenFont(HFONT hFont) {
	EnumChildWindows(m_hwnd, _SetFontProc, (LPARAM)hFont);
}

void BaseWindow::MainLoop()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

WindowInfoStruct BaseWindow::GetWindowInfo()
{
	TCHAR className[20] = TEXT("clsname:");
	StringCchPrintf(className+8, 10, TEXT("%d"), (int)this);

	WindowInfoStruct wi = { };
	wi.hCursor = LoadCursor(NULL, IDC_ARROW);
	wi.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wi.hIconSmall = LoadIcon(NULL, IDI_APPLICATION);
	wi.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wi.lpMenuName = NULL;
	wi.lpClassName = className;
	wi.style = CS_DBLCLKS;
	wi.cbClsExtra = wi.cbWndExtra = 0;
	return wi;
}

LRESULT BaseWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

ATOM BaseWindow::_RegisterWindow(BaseWindow *pThis)
{
	WNDCLASSEX wc = { };
	WindowInfoStruct wi = pThis->GetWindowInfo();

	wc.cbSize = sizeof(wc);
	wc.style = wi.style;
	wc.lpfnWndProc = BaseWindow::_WndProc;
	wc.cbClsExtra = wi.cbClsExtra;
	wc.cbWndExtra = wi.cbWndExtra;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = wi.hIcon;
	wc.hCursor = wi.hCursor;
	wc.hbrBackground = wi.hbrBackground;
	wc.lpszMenuName = wi.lpMenuName;
	wc.lpszClassName = wi.lpClassName;
	wc.hIconSm = wi.hIconSmall;
	return RegisterClassEx(&wc);
}

BOOL CALLBACK BaseWindow::_SetFontProc(HWND hwnd, LPARAM hFont) {
	SendMessage(hwnd, WM_SETFONT, hFont, TRUE);
	return TRUE;
}

LRESULT CALLBACK BaseWindow::_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BaseWindow *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
		pThis = (BaseWindow *)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)pThis);

		pThis->m_hwnd = hwnd;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else {
		pThis = (BaseWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
		
	if (pThis) {
		return pThis->HandleMessage(uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
