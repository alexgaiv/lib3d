#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "datatypes.h"
#include "glcontext.h"

enum CameraType {
	CAM_FREE,
	CAM_LAND
};

class Camera
{
public:
	Camera(CameraType type = CAM_LAND);

	void SetType(CameraType t) { type = t; }
	CameraType GetType() const { return type; }

	virtual Matrix44f GetViewMatrix();
	void ApplyTransform(GLRenderingContext *rc);
	void ResetTransform();

	Vector3f GetPosition() { return t; }
	void SetPosition(float x, float y, float z)
		{ t = Vector3f(x, y, z); changed = true; }
	void SetRotation(const Vector3f &x, const Vector3f &y, const Vector3f &z)
		{ this->x = x; this->y = y; this->z = z; changed = true; }

	void MoveX(float step);
	void MoveY(float step);
	void MoveZ(float step);

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
protected:
	Vector3f x, y, z, t;
	Matrix33f m;
	Matrix44f view;
	bool changed;

	CameraType type;
};

#endif // _CAMERA_H_