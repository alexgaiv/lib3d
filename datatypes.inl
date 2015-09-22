#ifndef _DATATYPES_INL_
#define _DATATYPES_INL_

#include <string.h>

#pragma region Point2

template<class T>
bool Point2<T>::operator==(const Point2<T> &p) const {
	return CmpReal(x, p.x) && CmpReal(y, p.y);
}

template<class T>
bool Point2<T>::operator!=(const Point2<T> &p) const {
	return !operator==(p);
}

#pragma endregion
#pragma region Point3

template<class T>
bool Point3<T>::operator==(const Point3<T> &p) const {
	return CmpReal(x, p.x) && CmpReal(y, p.y) && CmpReal(z, p.z);
}

template<class T>
bool Point3<T>::operator!=(const Point3<T> &p) const {
	return !operator==(p);
}

#pragma endregion
#pragma region Vector3

template<class T>
T Vector3<T>::Length() const {
	return sqrt(x*x + y*y + z*z);
}

template<class T>
T Vector3<T>::LengthSquared() const {
	return x*x+y*y+z*z;
}

template<class T>
void Vector3<T>::Normalize() {
	T l = Length();
	if (l == T(0)) return;
	l = T(1) / l;
	x *= l;
	y *= l;
	z *= l;
}

template<class T>
T Vector3<T>::Dot(Vector3<T> v1, Vector3<T> v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<class T>
Vector3<T> Vector3<T>::Cross(Vector3<T> v1, Vector3<T> v2) {
	return Vector3<T>(
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x);
}

template<class T>
bool Vector3<T>::operator==(const Vector3<T> &v) const {
	return CmpReal(x, v.x) && CmpReal(y, v.y) && CmpReal(z, v.z);
}

template<class T>
bool Vector3<T>::operator!=(const Vector3<T> &v) const {
	return !operator==(v);
}

template<class T>
Vector3<T> Vector3<T>::operator+(const Vector3<T> &v) const {
	return Vector3<T>(x + v.x, y + v.y, z + v.z);
}

template<class T>
Vector3<T> Vector3<T>::operator-(const Vector3<T> &v) const {
	return Vector3<T>(x - v.x, y - v.y, z - v.z);
}

template<class T>
Vector3<T> Vector3<T>::operator-() const {
	return Vector3<T>(-x, -y, -z);
}

template<class T>
Vector3<T> Vector3<T>::operator*(T scale) const {
	return Vector3<T>(x * scale, y * scale, z * scale);
}

template<class T>
Vector3<T> operator*(T scale, const Vector3<T> &v) {
	return v*scale;
}

template<class T>
Vector3<T> Vector3<T>::operator*(const Matrix33<T> &m) const {
	const T *d = m.data;
	return Vector3<T>(
		x*d[0] + y*d[3] + z*d[6],
		x*d[1] + y*d[4] + z*d[7],
		x*d[2] + y*d[5] + z*d[8]);
}

template<class T>
Vector3<T> Vector3<T>::operator/(T scale) const {
	return Vector3<T>(x / scale, y / scale, z / scale);
}

template<class T>
Vector3<T> &Vector3<T>::operator+=(const Vector3<T> &v) {
	x += v.x; y += v.y; z += v.z;
	return *this;
}

template<class T>
Vector3<T> &Vector3<T>::operator-=(const Vector3<T> &v) {
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

template<class T>
Vector3<T> &Vector3<T>::operator*=(T scale) {
	x *= scale; y *= scale; z *= scale;
	return *this;
}

template<class T>
Vector3<T> &Vector3<T>::operator*=(const Matrix33<T> &m) {
	return (*this = *this * m);
}

template<class T>
Vector3<T> &Vector3<T>::operator/=(T scale) {
	x /= scale; y /= scale; z /= scale;
	return *this;
}
#pragma endregion
#pragma region Vector4

template<class T>
void Vector4<T>::Cartesian() {
	if (w == T(0)) return;
	T w_inv = T(1) / w;
	x *= w_inv;
	y *= w_inv;
	z *= w_inv;
}

template<class T>
bool Vector4<T>::operator==(const Vector4<T> &v) const {
	return CmpReal(x, v.x) && CmpReal(y, v.y) &&
		CmpReal(z, v.z) && CmpReal(w, v.w);
}

template<class T>
bool Vector4<T>::operator!=(const Vector4<T> &v) const {
	return !operator==(v);
}

template<class T>
Vector4<T> Vector4<T>::operator*(const Matrix44<T> &m) const {
	const T *d = m.data;
	return Vector4<T>(
		x*d[0] + y*d[4] + z*d[8]  + w*d[12],
		x*d[1] + y*d[5] + z*d[9]  + w*d[13],
		x*d[2] + y*d[6] + z*d[10] + w*d[14],
		x*d[3] + y*d[7] + z*d[11] + w*d[15]);
}

template<class T>
Vector4<T> &Vector4<T>::operator*=(const Matrix44<T> &m) {
	return (*this = *this * m);
}
#pragma endregion
#pragma region Matrix33

template<class T>
Matrix33<T>::Matrix33(const T m[9]) {
	memcpy(data, m, sizeof(T)*9);
}

template<class T>
Matrix33<T>::Matrix33(const Matrix44<T> &m) {
	xAxis = m.xAxis;
	yAxis = m.yAxis;
	zAxis = m.zAxis;
}

template<class T>
template<class T2>
Matrix33<T>::Matrix33(const Matrix33<T2> &m) {
	for (int i = 0; i < 9; i++) {
		data[i] = T(m.data[i]);
	}
}

template<class T>
Matrix33<T> Matrix33<T>::GetTranspose()
{
	Matrix33<T> m1;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) m1.m[i][j] = m[j][i];
	return m1;
}

#define _DET2(m, n, p, q) (data[m]*data[n]-data[p]*data[q])

template<class T>
bool Matrix33<T>::GetInverse(Matrix33<T> &out)
{
	T d = Determinant();
	if (d == T(0)) return false;
	d = T(1) / d;

	T adj[9] = {
		_DET2(4,8,5,7)*d, _DET2(2,7,1,8)*d, _DET2(1,5,2,4)*d,
		_DET2(5,6,3,8)*d, _DET2(0,8,2,6)*d, _DET2(2,3,0,5)*d,
		_DET2(3,7,4,6)*d, _DET2(1,6,0,7)*d, _DET2(0,4,1,3)*d,
	};
	memcpy(out.data, adj, sizeof(T)*9);
	return true;
}

template<class T>
T Matrix33<T>::Determinant()
{
	return data[0]*_DET2(4,8,5,7) -
		   data[3]*_DET2(1,8,2,7) +
		   data[6]*_DET2(1,5,2,4);
}

#undef _DET2

template<class T>
void Matrix33<T>::LoadIdentity() {
	for (int i = 0; i < 9; i++)
		data[i] = T(0);
	data[0] = data[4] = data[8] = T(1);
}

template<class T>
Matrix33<T> Matrix33<T>::Identity() {
	Matrix33<T> m;
	m.data[0] = m.data[4] = m.data[8] = T(1);
	return m;
}

template<class T>
Matrix33<T> Matrix33<T>::Multiply(const Matrix33<T> &m, const Vector3<T> &v) {
	return v * m;
}

#ifndef D3D_SDK_VERSION
#define _MMAT1(i, j) \
	m[i+j] = a[i]*b[j] + a[i+3]*b[j+1] + a[i+6]*b[j+2]
#else
#define _MMAT1(i, j) \
	m[i+j] = b[i]*a[j] + b[i+3]*a[j+1] + b[i+6]*a[j+2]
#endif
#define _MMAT(i) _MMAT1(i, 0); _MMAT1(i, 3); _MMAT1(i, 6);

template<class T>
Matrix33<T> Matrix33<T>::Multiply(const Matrix33<T> &m1, const Matrix33<T> &m2) {
	Matrix33<T> res;
	const T *a = m1.data, *b = m2.data;
	T *m = res.data;
	_MMAT(0); _MMAT(1); _MMAT(2);
	return res;
}

#undef _MMAT1
#undef _MMAT

template<class T>
bool Matrix33<T>::operator==(const Matrix33<T> &m) const {
	for (int i = 0; i < 9; i++) {
		if (!CmpReal(data[i], m.data[i])) return false;
	}
	return true;
}

template<class T>
bool Matrix33<T>::operator!=(const Matrix33<T> &m) const {
	return !operator==(m);
}

template<class T>
Vector3<T> Matrix33<T>::operator*(const Vector3<T> &v) const {
	return v * *this;
}

template<class T>
Matrix33<T> Matrix33<T>::operator*(const Matrix33<T> &m) const {
	return Matrix33<T>::Multiply(*this, m);
}

template<class T>
Matrix33<T> &Matrix33<T>::operator*=(const Matrix33<T> &m) {
	return *this = Matrix33<T>::Multiply(*this, m);
}
#pragma endregion
#pragma region Matrix44

template<class T>
Matrix44<T>::Matrix44(const T m[16]) {
	memcpy(data, m, sizeof(T)*16);
}

template<class T>
Matrix44<T>::Matrix44(const Matrix33<T> &m) {
	wx = wy = wz = T(0); wt = T(1);
	xAxis = m.xAxis;
	yAxis = m.yAxis;
	zAxis = m.zAxis;
}

template<class T>
template<class T2>
Matrix44<T>::Matrix44(const Matrix44<T2> &m) {
	for (int i = 0; i < 16; i++)
		data[i] = T(m.data[i]);
}

template<class T>
void Matrix44<T>::SetRotation(const Matrix33<T> &m)
{
	xAxis = m.xAxis;
	yAxis = m.yAxis;
	zAxis = m.zAxis;
}

template<class T>
Matrix44<T> Matrix44<T>::GetTranspose()
{
	Matrix44<T> m1;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) m1.m[i][j] = m[j][i];
	return m1;
}

template<class T>
bool Matrix44<T>::GetInverse(Matrix44<T> &out)
{
	T d = Determinant();
	if (d == T(0)) return false;
	d = T(1) / d;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			Matrix33<T> m_ij;
			int im = 0;
			for (int p = 0; p < 4; p++) {
				if (p == i) continue;
				for (int q = 0; q < 4; q++) {
					if (q == j) continue;
					m_ij.data[im++] = this->m[q][p];
				}
			}
			int sign = (i + j) % 2 ? -1 : 1;
			out.m[i][j] = (T)sign * m_ij.Determinant() * d;
		}
	return true;
}

template<class T>
T Matrix44<T>::Determinant()
{
	T det;
	Matrix33<T> m;

	m.xAxis = xAxis;
	m.yAxis = yAxis;
	m.zAxis = zAxis;

	det  = data[15] * m.Determinant(); m.zAxis = translate;
	det -= data[11] * m.Determinant(); m.yAxis = zAxis;
	det += data[7]  * m.Determinant(); m.xAxis = yAxis;
	det -= data[3]  * m.Determinant();
	return det;
}

template<class T>
void Matrix44<T>::LoadIdentity() {
	for (int i = 0; i < 16; i++)
		data[i] = T(0);
	data[0] = data[5] = data[10] = data[15] = T(1);
}

template<class T>
Matrix44<T> Matrix44<T>::Identity() {
	Matrix44<T> m;
	m.data[0] = m.data[5] = m.data[10] = m.data[15] = T(1);
	return m;
}

#ifndef D3D_SDK_VERSION
#define _MMAT1(i, j) \
	m[i+j] = a[i]*b[j] + a[i+4]*b[j+1] + a[i+8]*b[j+2] + a[i+12]*b[j+3]
#else
#define _MMAT1(i, j) \
	m[i+j] = b[i]*a[j] + b[i+4]*a[j+1] + b[i+8]*a[j+2] + b[i+12]*a[j+3]
#endif
#define _MMAT(i) _MMAT1(i, 0); _MMAT1(i, 4); _MMAT1(i, 8); _MMAT1(i, 12);

template<class T>
Matrix44<T> Matrix44<T>::Multiply(const Matrix44<T> &m1, const Matrix44<T> &m2) {
	Matrix44<T> res;
	const T *a = m1.data, *b = m2.data;
	T *m = res.data;
	_MMAT(0); _MMAT(1); _MMAT(2); _MMAT(3);
	return res;
}

#undef _MMAT1
#undef _MMAT

template<class T>
bool Matrix44<T>::operator==(const Matrix44<T> &m) const {
	for (int i = 0; i < 9; i++) {
		if (!CmpReal(data[i], m.data[i])) return false;
	}
	return true;
}

template<class T>
bool Matrix44<T>::operator!=(const Matrix44<T> &m) const {
	return !operator==(m);
}

template<class T>
Vector4<T> Matrix44<T>::operator*(const Vector4<T> &v) const {
	return v * *this;
}

template<class T>
Matrix44<T> Matrix44<T>::operator*(const Matrix44<T> &m) const {
	return Matrix44<T>::Multiply(*this, m);
}

template<class T>
Matrix44<T> &Matrix44<T>::operator*=(const Matrix44<T> &m) {
	return *this = Matrix44<T>::Multiply(*this, m);
}
#pragma endregion
#pragma region Color4

template<class T>
bool Color4<T>::operator==(const Color4<T> &c) const {
	return CmpReal(r, c.r) && CmpReal(g, c.g) && CmpReal(b, c.b) && CmpReal(a, c.a);
}

template<class T>
bool Color4<T>::operator!=(const Color4<T> &c) const {
	return !operator==(c);
}
#pragma endregion
#pragma region Color3

template<class T>
bool Color3<T>::operator==(const Color3<T> &c) const {
	return CmpReal(r, c.r) && CmpReal(g, c.g) && CmpReal(b, c.b);
}

template<class T>
bool Color3<T>::operator!=(const Color3<T> &c) const {
	return !operator==(c);
}
#pragma endregion

#endif // _DATATYPES_INL_
