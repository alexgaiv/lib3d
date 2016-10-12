#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include "glwindow.h"
#include "camera.h"
#include "skybox.h"
#include "model.h"
#include "text2d.h"

class MainWindow : public GLWindow
{
public:
	MainWindow();
	void Update(int timeDelta);
private:
	Camera camera;
	Skybox *skybox;
	ProgramObject *mainShader;
	Model *sponza, *gun, *muzzle_flash, *crosshair;
	Text2D *text;

	bool fShowMuzzleFlash;
	bool fGunAnim;
	float gunAnim;

	WindowInfoStruct GetWindowInfo()
	{
		WindowInfoStruct wi = { };
		wi.hCursor = NULL;
		wi.hIcon = wi.hIconSmall = LoadIcon(NULL, IDI_APPLICATION);
		wi.lpClassName = "mainwindow";
		return wi;
	}

	void Shot();

	void OnCreate();
	void OnDisplay();
	void OnSize(int w, int h);
	void OnKeyDown(UINT keyCode) {
		if (keyCode == 27) DestroyWindow(m_hwnd);
	}
	void OnMouseDown(MouseButton btn, int x, int y);
	void OnMouseMove(UINT keyPressed, int x, int y);
	void OnMouseWheel(short delta, UINT keyPressed, int x, int y);
	void OnDestroy();
};

#endif // _MAIN_WINDOW_H_