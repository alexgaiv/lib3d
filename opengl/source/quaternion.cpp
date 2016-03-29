#include "quaternion.h"
#include "common.h"

Quaternion::Quaternion() {
	x = y = z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(const Vector3f &axis, float angleInDegrees)
{
	float a = DEG_TO_RAD(angleInDegrees) * 0.5f;
	float s = sin(a);
	float norm = axis.Length();
	if (norm < 0.00001f) norm = 0.00001f;

	x = s * axis.x / norm;
	y = s * axis.y / norm;
	z = s * axis.z / norm;
	w = cos(a);
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

float Quaternion::Norm() const {
	return sqrt(x*x + y*y + z*z + w*w);
}

float Quaternion::Angle() const {
	float n = sqrt(x*x + y*y + z*z);
	return 2.0f * RAD_TO_DEG(atan2(n, w));
}

Vector3f Quaternion::Axis() const {
	Vector3f axis(x, y, z);
	axis.Normalize();
	return axis;
}

Quaternion Quaternion::GetConjugate() const {
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::GetInverse() const {
	float n = x*x + y*y + z*z + w*w;
	if (n < 0.00001) n = 1.0f;
	return Quaternion(-x/n, -y/n, -z/n, w/n);
}

void Quaternion::Normalize() {
	float n = Norm();
	if (n == 0.0f) return;
	n = 1.0f / n;
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

void Quaternion::LoadIdentity() {
	x = y = z = 0.0f; w = 1.0f;
}

Quaternion Quaternion::Identity() {
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::Add(const Quaternion &q1, const Quaternion &q2) {
	return Quaternion(
		q1.x + q2.x,
		q1.y + q2.y,
		q1.z + q2.z,
		q1.w + q2.w);
}

Quaternion Quaternion::Subtract(const Quaternion &q1, const Quaternion &q2) {
	return Quaternion(
		q1.x - q2.x,
		q1.y - q2.y,
		q1.z - q2.z,
		q1.w - q2.w);
}

Quaternion Quaternion::Multiply(const Quaternion &q1, const Quaternion &q2) {
	return Quaternion(
		q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
		q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
		q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w,
		q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z);
}

Quaternion Quaternion::Multiply(const Quaternion &q, float scale) {
	return Quaternion(q.x * scale, q.y * scale, q.z * scale, q.w * scale);
}

Quaternion Quaternion::operator+(const Quaternion &q) const {
	return Quaternion::Add(*this, q);
}

Quaternion Quaternion::operator-(const Quaternion &q) const {
	return Quaternion::Subtract(*this, q);
}

Quaternion Quaternion::operator-() const {
	return Quaternion(-x, -y, -z, -w);
}

Quaternion Quaternion::operator*(const Quaternion &q) const {
	return Quaternion::Multiply(*this, q);
}

Quaternion Quaternion::operator*(float scale) const {
	return Quaternion::Multiply(*this, scale);
}

Quaternion Quaternion::operator/(float scale) const {
	return Quaternion(x / scale, y / scale, z / scale, w / scale);
}

Quaternion operator*(float scale, const Quaternion &q) {
	return Quaternion::Multiply(q, scale);
}

Quaternion &Quaternion::operator+=(const Quaternion &q) {
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

Quaternion &Quaternion::operator-=(const Quaternion &q) {
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

Quaternion &Quaternion::operator*=(const Quaternion &q) {
	*this = Quaternion::Multiply(*this, q);
	return *this;
}

Quaternion &Quaternion::operator*=(float scale) {
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
	return *this;
}

Quaternion &Quaternion::operator/=(float scale) {
	x /= scale;
	y /= scale;
	z /= scale;
	w /= scale;
	return *this;
}