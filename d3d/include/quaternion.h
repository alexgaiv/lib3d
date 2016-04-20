#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "datatypes.h"

class Quaternion
{
public:
	float x, y, z, w;
	
	Quaternion();
	Quaternion(const Vector3f &axis, float angleInDegrees);
	Quaternion(float x, float y, float z, float w);

	float Norm() const;
	float Angle() const;
	Vector3f Axis() const;

	Quaternion GetConjugate() const;
	Quaternion GetInverse() const;
	void Normalize();
	void LoadIdentity();

	template<class TMatrix> void ToMatrix(TMatrix &m) const;
	
	static Quaternion Identity();
	static Quaternion Add(const Quaternion &q1, const Quaternion &q2);
	static Quaternion Subtract(const Quaternion &q1, const Quaternion &q2);
	static Quaternion Multiply(const Quaternion &q1, const Quaternion &q2);
	static Quaternion Multiply(const Quaternion &q1, float scale);

	Quaternion operator+(const Quaternion &q) const;
	Quaternion operator-(const Quaternion &q) const;
	Quaternion operator-() const;
	Quaternion operator*(const Quaternion &q) const;
	Quaternion operator*(float scale) const;
	Quaternion operator/(float scale) const;
	friend Quaternion operator*(float scale, const Quaternion &q);

	Quaternion &operator+=(const Quaternion &q);
	Quaternion &operator-=(const Quaternion &q);
	Quaternion &operator*=(const Quaternion &q);
	Quaternion &operator*=(float scale);
	Quaternion &operator/=(float scale);
};

template<class TMatrix>
void Quaternion::ToMatrix(TMatrix &m) const
{
	float wx, wy, wz, xx, xy, xz, yy, yz, zz, x2, y2, z2;
	float s = 2.0f / Norm();
	x2 = x * s;
	y2 = y * s;
	z2 = z * s;
	wx = w * x2; wy = w * y2; wz = w * z2;
	xx = x * x2; xy = x * y2; xz = x * z2;
	yy = y * y2; yz = y * z2; zz = z * z2;

	m.xAxis.x = 1.0f-(yy+zz); m.yAxis.x = xy - wz;      m.zAxis.x = xz + wy;
	m.xAxis.y = xy + wz;      m.yAxis.y = 1.0f-(xx+zz); m.zAxis.y = yz - wx;
	m.xAxis.z = xz - wy;      m.yAxis.z = yz + wx;      m.zAxis.z = 1.0f-(xx+yy);
}

#endif // _QUATERNION_H_