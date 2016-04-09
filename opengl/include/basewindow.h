#ifndef _BASE_WINDOW_H_
#define _BASE_WINDOW_H_

#include <Windows.h>
#include <strsafe.h>
#include "sharedptr.h"

#define BEGIN_MSG_MAP protected: LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {\
	switch(uMsg) {
#define MSG_HANDLER(msg, proc)\
	case msg:\
		return proc(uMsg, wParam, lParam);
#define END_MSG_MAP }\
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);}

inline HFONT CreateGuiFont()
{
	NONCLIENTMETRICS ncm = { };
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, FALSE);
	return CreateFontIndirect(&ncm.lfMessageFont);
}

struct WindowInfoStruct
{
	HCURSOR hCursor;
	HICON   hIcon;
	HICON   hIconSmall;
	HBRUSH  hbrBackground;
	LPCTSTR lpMenuName;
	LPCTSTR lpClassName;
	UINT    style;
	int     cbClsExtra;
	int     cbWndExtra;
};

class BaseWindow
{
public:
	BaseWindow() : m_hwnd(NULL) { }

	BOOL Create(
		LPCTSTR lpWindowName,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		DWORD dwExStyle = 0,
		HMENU hMenu = NULL,
		HWND hWndParent = NULL);
	
	void Show(int nCmdShow) { ShowWindow(m_hwnd, nCmdShow); }
	void Destroy() { DestroyWindow(m_hwnd); }
	void SetChildrenFont(HFONT hFont);
	void MainLoop();

	HWND Hwnd() { return m_hwnd; }
	operator HWND() { return m_hwnd; }
protected:
	HWND m_hwnd;

	virtual WindowInfoStruct GetWindowInfo();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	struct Shared
	{
		HWND hwnd;
		ATOM classAtom;
		Shared() : hwnd(NULL), classAtom(0) { }
		~Shared();
	};

	my_shared_ptr<Shared> ptr;

	static ATOM registerWindow(BaseWindow *pThis);
	static BOOL CALLBACK setFontProc(HWND hwnd, LPARAM hFont);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // _BASE_WINDOW_H_
