#include "camera.h"
#include "quaternion.h"

Camera::Camera(CameraType type) : type(type), changed(true) {
	x.x = y.y = z.z = 1.0f;
}

Matrix44f Camera::GetViewMatrix()
{
	if (!changed) return view;

	x.Normalize();
	y = Cross(z, x);
	y.Normalize();
	x = Cross(y, z);
	x.Normalize();

	view.xAxis = Vector3f(x.x, y.x, z.x);
	view.yAxis = Vector3f(x.y, y.y, z.y);
	view.zAxis = Vector3f(x.z, y.z, z.z);
	view.translate = Vector3f(-Dot(t, x), -Dot(t, y), -Dot(t, z));

	changed = false;
	return view;
}

void Camera::ApplyTransform(GLRenderingContext *rc) {
	rc->MultModelView(GetViewMatrix());
}

void Camera::ResetTransform() {
	x = y = z = t = Vector3f(0.0f);
	x.x = y.y = z.z = 1.0f;
	m.LoadIdentity();
	view.LoadIdentity();
	changed = true;
}

void Camera::MoveX(float step) {
	t += (type == CAM_FREE ? x : Vector3f(x.x, 0.0f, x.z)) * step;
	changed = true;
}

void Camera::MoveY(float step) {
	t += y * step;
	changed = true;
}

void Camera::MoveZ(float step) {
	t += (type == CAM_FREE ? z : Vector3f(z.x, 0.0f, z.z)) * step;
	changed = true;
}

void Camera::RotateX(float angle)
{
	Quaternion(x, angle).ToMatrix(m);
	y *= m;
	z *= m;
	changed = true;
}

void Camera::RotateY(float angle)
{
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), angle).ToMatrix(m);
	x *= m;
	z *= m;
	changed = true;
}

void Camera::RotateZ(float angle)
{
	Quaternion(z, angle).ToMatrix(m);
	x *= m;
	y *= m;
	changed = true;
}