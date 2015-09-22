#ifndef _D3DWINDOW_H_
#define _D3DWINDOW_H_

#include <Windows.h>
#include <d3d9.h>

struct WindowInfoStruct
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

class D3DWindow
{
public:
	HWND m_hwnd;
	IDirect3D9 *m_d3d;
	IDirect3DDevice9 *m_dev;

	D3DWindow() : m_hwnd(NULL), m_d3d(NULL), m_dev(NULL),
		bFullScreen(false), bLost(false) { }

	HWND CreateParam(
		LPCTSTR lpCaption,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		DWORD dwExStyle = 0);

	HWND CreateFullScreen(LPCTSTR lpCaption);

	void Show(int nCmdShow) { ShowWindow(m_hwnd, nCmdShow); }

	void RedrawWindow() {
		OnDisplay();
		m_dev->Present(0, 0, 0, 0);
	}
	
	void MainLoop();
protected:
	virtual WindowInfoStruct GetWindowInfo();

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
	virtual void OnDeviceLost() { }
	virtual void OnDeviceActivated() { }
	virtual void OnDestroy() { }
	virtual void OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { }
private:
	bool bFullScreen, bLost;
	
	static ATOM _RegisterWindow(D3DWindow *pThis);
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT _HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	D3DPRESENT_PARAMETERS _GetPresentParams();
	bool _InitD3D();
};

#endif // _D3DWINDOW_H_