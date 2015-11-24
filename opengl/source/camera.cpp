#include "camera.h"
#include "quaternion.h"
#include "global.h"

Camera::Camera(CameraType type) : type(type) {
	x.x = y.y = z.z = 1.0f;
}

Matrix44f Camera::GetViewMatrix()
{
	x.Normalize();
	y = Vector3f::Cross(z, x);
	y.Normalize();
	x = Vector3f::Cross(y, z);
	x.Normalize();

	Matrix44f view;
	view.xAxis = Vector3f(x.x, y.x, z.x);
	view.yAxis = Vector3f(x.y, y.y, z.y);
	view.zAxis = Vector3f(x.z, y.z, z.z);
	view.translate = Vector3f(-t.Dot(t, x), -t.Dot(t, y), -t.Dot(t, z));
	return view;
}

void Camera::ApplyTransform() {
	Global::MultModelView(GetViewMatrix());
}

void Camera::MoveX(float step) {
	t += (type == CAM_FREE ? x : Vector3f(x.x, 0.0f, x.z)) * step;
}

void Camera::MoveY(float step) {
	t += y * step;
}

void Camera::MoveZ(float step) {
	t += (type == CAM_FREE ? z : Vector3f(z.x, 0.0f, z.z)) * step;
}

void Camera::RotateX(float angle)
{
	Quaternion(x, angle).ToMatrix(m);
	y *= m;
	z *= m;
}

void Camera::RotateY(float angle)
{
	Vector3f axis = type == CAM_FREE ? y : Vector3f(0.0f, 1.0f, 0.0f);
	Quaternion(axis, angle).ToMatrix(m);
	x *= m;
	z *= m;
}

void Camera::RotateZ(float angle)
{
	Quaternion(z, angle).ToMatrix(m);
	x *= m;
	y *= m;
}