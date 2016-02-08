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

Matrix44f LookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up)
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

Vector3f Project(Vector3f objPos, const Matrix44f &modelview, const Matrix44f &projection, int viewport[4])
{
	Vector4f v = projection * modelview * objPos;
	v.Cartesian();
	return Vector3f(
		(v.x*0.5f + 0.5f)*viewport[2] + viewport[0],
		(v.y*0.5f + 0.5f)*viewport[3] + viewport[1],
		(1.0f + v.z) * 0.5f);
}

Vector3f UnProject(Vector3f winPos, const Matrix44f &modelview, const Matrix44f &projection, int viewport[4])
{
	Vector4f v(
		(winPos.x - viewport[0]) / viewport[2] * 2.0f - 1.0f,
		(winPos.y - viewport[1]) / viewport[3] * 2.0f - 1.0f,
		2.0f * winPos.z - 1.0f,
		1.0f
	);

	Matrix44f m_inv = (projection * modelview).GetInverse();
	v *= m_inv;
	v.Cartesian();
	return v;
}