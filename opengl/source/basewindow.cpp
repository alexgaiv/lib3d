#include "BaseWindow.h"

BaseWindow::Shared::~Shared() {
	if (IsWindow(hwnd)) {
		SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
		DestroyWindow(hwnd);
		UnregisterClass(MAKEINTATOM(classAtom), GetModuleHandle(NULL));
	}
}

BOOL BaseWindow::Create(LPCTSTR lpWindowName, int x, int y, int width, int height,
	DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HWND hWndParent)
{
	ATOM classAtom = registerWindow(this);

	m_hwnd = CreateWindowEx(dwExStyle, MAKEINTATOM(classAtom), lpWindowName, dwStyle,
		x, y, width, height, hWndParent, hMenu, GetModuleHandle(NULL), this);

	ptr = my_shared_ptr<Shared>(new Shared);
	ptr->hwnd = m_hwnd;
	ptr->classAtom = classAtom;

	return m_hwnd ? TRUE : FALSE;
}

void BaseWindow::SetChildrenFont(HFONT hFont) {
	EnumChildWindows(m_hwnd, setFontProc, (LPARAM)hFont);
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

ATOM BaseWindow::registerWindow(BaseWindow *pThis)
{
	WNDCLASSEX wc = { };
	WindowInfoStruct wi = pThis->GetWindowInfo();

	wc.cbSize = sizeof(wc);
	wc.style = wi.style;
	wc.lpfnWndProc = BaseWindow::wndProc;
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

BOOL CALLBACK BaseWindow::setFontProc(HWND hwnd, LPARAM hFont) {
	SendMessage(hwnd, WM_SETFONT, hFont, TRUE);
	return TRUE;
}

LRESULT CALLBACK BaseWindow::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
