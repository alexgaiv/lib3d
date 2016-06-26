#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "datatypes.h"
#include "glcontext.h"

enum CameraType {
	CAM_FREE = 1,
	CAM_LAND = 2
};

class Camera
{
public:
	CameraType type;
	Camera(CameraType type = CAM_LAND);

	virtual Matrix44f GetViewMatrix();
	void ApplyTransform(GLRenderingContext *rc);
	void ResetTransform();

	Vector3f GetPosition() { return t; }
	void SetPosition(float x, float y, float z)
		{ t = Vector3f(x, y, z); }
	void SetRotation(const Vector3f &x, const Vector3f &y, const Vector3f &z)
		{ this->x = x; this->y = y; this->z = z; }

	void MoveX(float step);
	void MoveY(float step);
	void MoveZ(float step);

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
protected:
	Vector3f x, y, z, t;
	Matrix33f m;
};

#endif // _CAMERA_H_