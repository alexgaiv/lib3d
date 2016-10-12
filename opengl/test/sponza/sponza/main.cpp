#include "mainwindow.h"
#include "gameloop.h"
#include "stringhelp.h"
using namespace strhlp;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	SetCurrentDirectory("../sponza");
	MainWindow wnd;
	wnd.Show(SW_SHOW);

	GameLoop loop(wnd);
	loop.RestrictFps(true, 30);
	//loop.RestrictFps(false);
	loop.sleep1 = true;
	loop.Start();

	return 0;
}
