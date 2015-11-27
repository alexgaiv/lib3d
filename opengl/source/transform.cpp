#include "transform.h"
#include "quaternion.h"

Matrix44f Rotate(float angle, float x, float y, float z)
{
	Quaternion q(Vector3f(x, y, z), angle);
	Matrix44f ret;
	q.ToMatrix(ret);
	return ret;
}

Matrix44f Scale(float x, float y, float z)
{
	Matrix44f ret;
	ret.m[0][0] = x;
	ret.m[1][1] = y;
	ret.m[2][2] = z;
	return ret;
}

Matrix44f Translate(float x, float y, float z)
{
	Matrix44f ret;
	ret.translate = Vector3f(x, y, z);
	return ret;
}

Matrix44f Frustum(float left, float right, float bottom,
	float top, float zNear, float zFar)
{
	Matrix44f ret;
	ret.m[0][0] = 2.0f*zNear / (right - left);
	ret.m[1][1] = 2.0f*zNear / (top - bottom);
	ret.m[2][0] = (right + left) / (right - left);
	ret.m[2][1] = (top + bottom) / (top - bottom);
	ret.m[2][2] = -(zFar + zNear) / (zFar - zNear);
	ret.m[2][3] = -1.0f;
	ret.m[3][2] = -(2*zFar*zNear) / (zFar - zNear);
	ret.m[3][3] = 0.0f;
	return ret;
}

Matrix44f Ortho(float left, float right, float bottom,
	float top, float zNear, float zFar)
{
	Matrix44f ret;
	float dx = 1.0f / (right - left);
	float dy = 1.0f / (top - bottom);
	float dz = 1.0f / (zFar - zNear);

	ret.m[0][0] = 2.0f*dx;
	ret.m[1][1] = 2.0f*dy;
	ret.m[2][2] = -2.0f*dz;
	ret.m[3][0] = -(right + left) * dx;
	ret.m[3][1] = -(top + bottom) * dy;
	ret.m[3][2] = -(zFar + zNear) * dz;
	return ret;
}

Matrix44f Ortho2D(float left, float right, float bottom, float top)
{
	return Ortho(left, right, bottom, top, -1.0f, 1.0f);
}

Matrix44f Perspective(float fovY, float aspect, float zNear, float zFar)
{
	float fh = tan(fovY * (float)M_PI / 360.0f) * zNear;
	float fw = fh * aspect;
	return Frustum(-fw, fw, -fh, fh, zNear, zFar);
}

Matrix44f LookAt(Vector3f eye, Vector3f center, Vector3f up)
{
	Matrix44f ret;
	Vector3f z = eye - center;
	Vector3f x = Cross(up, z);
	Vector3f y = Cross(z, x);

	x.Normalize();
	y.Normalize();
	z.Normalize();

	ret.xAxis = Vector3f(x.x, y.x, z.x);
	ret.yAxis = Vector3f(x.y, y.y, z.y);
	ret.zAxis = Vector3f(x.z, y.z, z.z);

	ret.translate = Matrix33f(ret) * eye;
	return ret;
}