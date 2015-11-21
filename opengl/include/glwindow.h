#ifndef _GLWINDOW_H_
#define _GLWINDOW_H_

#include "common.h"

struct GLWindowInfoStruct
{
	HCURSOR hCursor;
	HICON hIcon;
	HICON hIconSm;
	HBRUSH hbrBackground;
	LPCTSTR lpszMenuName;
	LPCTSTR lpszClassName;
};

enum MouseButton { NONE, LBUTTON, MBUTTON, RBUTTON };

enum KeyModifiers
{
	KM_CTRL    = MK_CONTROL,
	KM_SHIFT   = MK_SHIFT,
	KM_LBUTTON = MK_LBUTTON,
	KM_MBUTTON = MK_MBUTTON,
	KM_RBUTTON = MK_RBUTTON,
	KM_ANY_BUTTON = MK_LBUTTON|MK_MBUTTON|MK_RBUTTON
};

class GLWindow
{
public:
	HWND  m_hwnd;
	HDC   m_hdc;
	HGLRC m_hrc;

	GLWindow();
	virtual ~GLWindow();

	HWND CreateParam(
		LPCTSTR lpCaption,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		DWORD dwExStyle = 0,
		HWND hParent = NULL);

	void CreateFullScreen(LPCTSTR lpCaption);

	void Show(int nCmdShow) {
		ShowWindow(m_hwnd, (bFullScreen && nCmdShow == SW_MAXIMIZE) ? SW_SHOW : nCmdShow);
	}

	void RedrawWindow() {
		OnDisplay();
		SwapBuffers(m_hdc);
	}
	
	void MainLoop();
protected:
	virtual GLWindowInfoStruct GetWindowInfo();
	virtual PIXELFORMATDESCRIPTOR GetDCPixelFormat();

	virtual void OnCreate() { }
	virtual void OnDisplay() { }
	virtual void OnSize(int w, int h) { }
	virtual void OnMouseDown(MouseButton button, int x, int y) { }
	virtual void OnMouseUp(MouseButton button, int x, int y) { }
	virtual void OnMouseDblClick(MouseButton button, int x, int y) { }
	virtual void OnMouseMove(UINT keysPressed, int x, int y) { }
	virtual void OnMouseWheel(short delta, UINT keysPressed, int x, int y) { }
	virtual void OnKeyDown(UINT keyCode) { }
	virtual void OnKeyUp(UINT keyCode) { }
	virtual void OnChar(char charCode) { }
	virtual void OnTimer() { }
	virtual void OnDestroy() { }
	virtual void OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { }
private:
	bool bFullScreen;
	bool bDummy;
	
	static ATOM _RegisterWindow(GLWindow *pThis);
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT _HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void _InitRC();
	bool _CreateCompabilityContext();
	void _ChangeDisplaySettings();
};

#endif // _GLWINDOW_H_