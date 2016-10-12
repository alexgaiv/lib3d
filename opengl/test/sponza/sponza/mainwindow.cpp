#include "mainwindow.h"
#include "transform.h"
#include <strsafe.h>
#include <time.h>

MainWindow::MainWindow()
{
	this->Create("Sponza", CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700);
	//this->CreateFullScreen("Sponza");
	//EnableVsync(true);

	fShowMuzzleFlash = false;
	fGunAnim = false;
	gunAnim = 0.0f;
}

void MainWindow::Shot()
{
	if (fGunAnim) return;
	PlaySound("gun.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
	fGunAnim = true;
	fShowMuzzleFlash = true;
}

void MainWindow::OnCreate()
{
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.82f, 0.85f, 0.96f, 1.0f);

	const char *sides[6] =
	{
		"textures/skybox/ft.tga",
		"textures/skybox/bk.tga",
		"textures/skybox/up.tga",
		"textures/skybox/dn.tga",
		"textures/skybox/lf.tga",
		"textures/skybox/rt.tga",
	};

	skybox = new Skybox(m_rc, sides);
	mainShader = new ProgramObject(m_rc, "shaders/main.vert.glsl", "shaders/main.frag.glsl");
	gun = new Model(m_rc);
	muzzle_flash = new Model(m_rc);
	crosshair = new Model(m_rc);
	sponza = new Model(m_rc);

	Font2D font("fonts/font.fnt");
	font.SetColor(Color4f(1));
	text = new Text2D(m_rc, font);
	
	char dir[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, dir);
	SetCurrentDirectory("sponza_obj");
	sponza->LoadRaw("sponza.raw");
	sponza->shader = *mainShader;
	sponza->scale = Vector3f(0.2f);
	SetCurrentDirectory(dir);

	gun->LoadRaw("models/gun.raw");
	gun->shader = *mainShader;
	gun->location = Vector3f(0.7f, -2.0f, -2.0f);
	gun->scale = Vector3f(-1, 1, -1);

	muzzle_flash->LoadRaw("models/quad.raw");
	muzzle_flash->meshes[0].material = *m_rc->materials("materials.mtl", "muzzle");
	muzzle_flash->shader = *mainShader;
	muzzle_flash->location = Vector3f(0.72f, -0.26f, -3.5f);

	crosshair->AddMesh(muzzle_flash->meshes[0]);
	crosshair->meshes[0].material = *m_rc->materials("materials.mtl", "crosshair");
	crosshair->shader = *mainShader;
	crosshair->scale = Vector3f(4.0f);

	for (int i = 0, n = sponza->meshes.size(); i < n; i++) {
		sponza->meshes[i].ComputeBoundingBox();
	}

	mainShader->Uniform("ColorMap", 0);
	mainShader->Uniform("NormalMap", 1);
	mainShader->Uniform("SpecularMap", 2);
	mainShader->Uniform("OpacityMask", 4);

	camera.SetPosition(0, 20, 0);
	camera.RotateY(90);
	camera.MoveZ(-40);
	camera.SetType(CAM_FREE);
}

void MainWindow::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_rc->PushModelView();
		gun->Draw();

		camera.ApplyTransform(m_rc);
		skybox->Draw();

		static int viewLoc = mainShader->GetUniformLocation("View");
		Matrix44f view = m_rc->GetModelView();
		mainShader->Use();
		glUniformMatrix4fv(viewLoc, 1, FALSE, view.data);

		int drawCalls = sponza->Draw();
		WCHAR buf[20] = L"";
		StringCchPrintfW(buf, 20, L"Meshes: %d", drawCalls);
		text->SetText(buf);
		
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	
		if (fShowMuzzleFlash) {
			m_rc->PushModelView();
				m_rc->SetModelView(Matrix44f::Identity());
				muzzle_flash->Draw();
			m_rc->PopModelView();
		}

		m_rc->PushModelView();
		m_rc->PushProjection();
			float viewport[4] = { };
			glGetFloatv(GL_VIEWPORT, viewport);
			m_rc->SetProjection(Ortho2D(0, viewport[2], viewport[3], 0));
			m_rc->SetModelView(Matrix44f::Identity());

			crosshair->location = Vector3f(viewport[2]*0.5f, viewport[3]*0.5f, 0);
			crosshair->Draw();
		m_rc->PopModelView();
		m_rc->PopProjection();

		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		text->Draw(10, 10);
	m_rc->PopModelView();
}

void MainWindow::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	m_rc->SetProjection(Perspective(45.0f, (float)w/h, 0.1f, 1000.0f));
}

void MainWindow::Update(int timeDelta)
{
	float kt = timeDelta / 16.0f;

	static int dir = 1;
	static float anim = 0.0f;
	anim += 0.01f * dir * kt;

	if (fGunAnim)
	{
		static int dir = 1;
		static int t = 0;
		float maxAnim = 15.0f;

		if (dir == 1)
			gunAnim += 0.5f * (maxAnim - gunAnim) * kt;
		else gunAnim -= 1.0f * kt;

		gun->rotation = Quaternion(Vector3f(1,0,0), gunAnim);

		if (gunAnim >= maxAnim - 0.4f) {
			dir = -1;
			gunAnim = maxAnim;
		}
		else if (dir == -1 && gunAnim <= 0.0f) {
			dir = 1;
			fGunAnim = false;
			fShowMuzzleFlash = false;
			t = 0;
			gun->rotation = Quaternion::Identity();
		}

		if (t >= 70) {
			fShowMuzzleFlash = false;
			t = 0;
		}
		t += timeDelta;
	}

	const float step = 0.07f * timeDelta;
	float dx = 0.0f, dz = 0.0f;

	if (GetAsyncKeyState('W'))
		dz = -step;
	else if (GetAsyncKeyState('S'))
		dz = step;

	if (GetAsyncKeyState('A'))
		dx = -step;
	else if (GetAsyncKeyState('D'))
		dx = step;
	
	camera.MoveX(dx);
	camera.MoveZ(dz);
}

void MainWindow::OnMouseDown(MouseButton btn, int x, int y)
{
	Shot();
}

void MainWindow::OnMouseMove(UINT keyPressed, int x, int y)
{
	SetCursor(NULL);
	static bool centerCursor = false;
	const float angle = 0.04f;

	if (centerCursor) {
		centerCursor = false;
		return;
	}

	RECT clientRect = { };
	GetClientRect(m_hwnd, &clientRect);
	int centerX = clientRect.right / 2;
	int centerY = clientRect.bottom / 2;

	camera.RotateX(angle * (centerY - y));
	camera.RotateY(angle * (centerX - x));

	POINT cursorPos = { centerX, centerY };
	ClientToScreen(m_hwnd, &cursorPos);
	SetCursorPos(cursorPos.x, cursorPos.y);
	centerCursor = true;
}

void MainWindow::OnMouseWheel(short delta, UINT keyPressed, int x, int y)
{
}

void MainWindow::OnDestroy()
{
	delete skybox;
	delete mainShader;
	delete sponza;
	delete gun;
	delete muzzle_flash;
	delete crosshair;
	delete text;
	PostQuitMessage(0);
}