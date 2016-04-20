#ifndef _VIEWER3D_
#define _VIEWER3D_

#include "common.h"
#include "glcontext.h"
#include "datatypes.h"
#include "quaternion.h"

class TrackballCamera
{
public:
	Quaternion qRotation;
	TrackballCamera(GLRenderingContext *rc);

	Matrix44f GetViewMatrix();
	void ApplyTransform();

	void ResetView();
	void BeginPan(int winX, int winY);
	void BeginRotate(int winX, int winY);
	void Pan(int winX, int winY);
	void Rotate(int winX, int winY);
	void Zoom(float scale);
	void SetScale(float scale);
	float GetScale() const { return scale; };

	void SetPerspective(float fovy, float zNear, float zFar,
		Point3f center, int winWidth, int winHeight);

	void SetOrtho(float left, float right, float bottom, float top,
		float zNear, float zFar, int winWidth, int winHeight);

	void SetRotationSpeed(float factor)
		{ isConstSpeed = false; view.s = 1.0f / factor; }
	void SetConstRotationSpeed(float rotSpeed)
		{ isConstSpeed = true; constSpeedValue = 1.0f / rotSpeed; }
private:
	GLRenderingContext *rc;
	bool isConstSpeed;
	float constSpeedValue;

	struct { float fw, fh, w, h, s; } view;
	float scale;
	Matrix44f rot, trs, matr, matr_inv;
	Vector3f from, to, last;
	bool changed;

	Vector3f pos(const Vector3f &p, int x, int y);
	void calcMatr();
};

#endif // _VIEWER3D_