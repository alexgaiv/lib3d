#include "viewer3d.h"

Viewer3D::Viewer3D()
	: isConstSpeed(false), constSpeedValue(0.0f)
{
	view.fw = view.fw = view.s = 1.0f;
	view.w = view.h = 0.0f;
	rot.wt = 1.0f;
	ResetView();
}

Matrix44f Viewer3D::Modelview() {
	qRotation.ToMatrix(rot);
	rot.xAxis *= scale;
	rot.yAxis *= scale;
	rot.zAxis *= scale;
	return rot * trs;
}

void Viewer3D::ApplyTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(Modelview().data);
}

void Viewer3D::ResetView() {
	qRotation.LoadIdentity();
	rot.SetRotation(Matrix33f::Identity());
	trs.LoadIdentity();
	scale = 1.0f;
	changed = true;
}

void Viewer3D::BeginPan(int winX, int winY) {
	last = pos(trs.translate, winX, winY);
}

void Viewer3D::BeginRotate(int winX, int winY) {
	from.x = winX * view.fw - 1.0f;
	from.y = winY * view.fh - 1.0f;
}

void Viewer3D::Pan(int winX, int winY)
{
	Vector3f p = pos(trs.translate, winX, winY);
	trs.translate += p - last;
	last = p;
}

void Viewer3D::Rotate(int winX, int winY)
{
	to.x = winX * view.fw - 1.0f;
	to.y = winY * view.fh - 1.0f;

	Vector3f delta = to - from;
	float length = delta.Length();
	if (length < 1e-5) return;
	delta /= length;

	float rotDist = isConstSpeed ? constSpeedValue : 
		0.5f * max(view.w, view.h) * view.s;
	float angle = length / rotDist * 180.0f;
	Vector3f axis = Vector3f(delta.y, delta.x, 0.0f);

	qRotation = Quaternion(axis, angle) * qRotation;
	from = to;
	changed = true;
}

void Viewer3D::ZoomIn(float scale) {
	this->scale *= scale;
	changed = true;
}

void Viewer3D::ZoomOut(float scale) {
	this->scale /= scale;
	changed = true;
}

void Viewer3D::SetScale(float scale) {
	this->scale = scale;
	changed = true;
}

void Viewer3D::SetOrtho(float left, float right, float bottom, float top,
	float zNear, float zFar, int winWidth, int winHeight)
{
	view.w = abs(right - left);
	view.h = abs(bottom - top);
	view.fw = view.w / winWidth;
	view.fh = view.h / winHeight;
	rot.translate = Vector3f();

	glMatrixMode(GL_PROJECTION);
	glOrtho(left, right, bottom, top, zNear, zFar);
	changed = true;
}

void Viewer3D::SetPerspective(float fovy, float zNear, float zFar,
	Point3f center, int winWidth, int winHeight)
{
	float aspect = (float)winWidth / (float)winHeight;
	view.h = tan(fovy * (float)M_PI / 360.0f) * zNear;
	view.w = view.h * aspect;

	glMatrixMode(GL_PROJECTION);
	glFrustum(-view.w, view.w, -view.h, view.h, zNear, zFar);

	view.w *= 2.0f; view.h *= 2.0f;
	view.fw = view.w / winWidth;
	view.fh = view.h / winHeight;
	rot.translate = Vector3f(center.x, center.y, center.z);
	changed = true;
}

Vector3f Viewer3D::pos(const Vector3f &p, int x, int y)
{
	float viewport[4] = { };
	glGetFloatv(GL_VIEWPORT, viewport);
	if (changed) calcMatr();

	Vector4f v = Vector4f(p) * matr;
	v.Cartesian();
	float winZ = (1.0f + v.z) * 0.5f;

	v.x = ((float)x - viewport[0]) / viewport[2]*2.0f - 1.0f;
	v.y = ((viewport[3] - (float)y) - viewport[1]) / viewport[3]*2.0f - 1.0f;
	v.z = 2.0f*winZ - 1.0f;
	v.w = 1.0f;
	v *= matr_inv;
	v.Cartesian();
	return Vector3f(v);
}

void Viewer3D::calcMatr()
{
	Matrix44f projection;
	glGetFloatv(GL_PROJECTION_MATRIX, projection.data);
	qRotation.ToMatrix(rot);

	matr = projection * rot;
	matr.xAxis *= scale;
	matr.yAxis *= scale;
	matr.zAxis *= scale;

	matr.GetInverse(matr_inv);
	changed = false;
}