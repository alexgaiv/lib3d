#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "common.h"
#include "glwindow.h"

#pragma comment(lib, "Winmm.lib")

class GameLoop
{
public:
	GameLoop(GLWindow &window) : wnd(&window), msPerFrame(16.0)
	{ }

	void RestrictFps(bool restrictFps, int maxFps = 0)
	{
		if (restrictFps) {
			msPerFrame = maxFps == 0 ? 16 : 1000 / maxFps;
		}
		else msPerFrame = 0.0;
	}

	void Start()
	{
		timeBeginPeriod(1);

		MSG msg = { };
		int lastTime = timeGetTime();
		while (msg.message != WM_QUIT)
		{
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) return;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			int curTime = timeGetTime();
			int dt = curTime - lastTime;

			if (wnd->IsVsyncEnabled() || dt >= msPerFrame)
			{
				wnd->Update((int)dt);
				wnd->Redraw();
				lastTime = curTime;
			}
			Sleep(1);
		}

		timeEndPeriod(1);
	}
private:
	GLWindow *wnd;
	double msPerFrame;
};

#endif // _GAMELOOP_H_