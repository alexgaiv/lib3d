#ifndef _DATATYPES_INL_
#define _DATATYPES_INL_

#include <string.h>

#pragma endregion
#pragma region Vector2

template<class T>
T Vector2<T>::Length() const {
	return sqrt(x*x + y*y);
}

template<class T>
T Vector2<T>::LengthSquared() const {
	return x*x + y*y;
}

template<class T>
void Vector2<T>::Normalize() {
	T f = T(1) / Length();
	x *= f; y *= f;
}

template<class T>
bool Vector2<T>::operator==(const Vector2<T> &v) const {
	return CmpReal(x, v.x) && CmpReal(y, v.y);
}

template<class T>
bool Vector2<T>::operator!=(const Vector2<T> &v) const {
	return !operator==(v);
}

template<class T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &v) const {
	return Vector2<T>(x + v.x, y + v.y);
}

template<class T>
Vector2<T> Vector2<T>::operator-(const Vector2<T> &v) const {
	return Vector2<T>(x - v.x, y - v.y);
}

template<class T>
Vector2<T> Vector2<T>::operator-() const {
	return Vector2<T>(-x, -y);
}

template<class T>
Vector2<T> Vector2<T>::operator*(T scale) const {
	return Vector2<T>(x * scale, y * scale);
}

template<class T>
Vector2<T> operator*(T scale, const Vector2<T> &v) {
	return v*scale;
}

template<class T>
Vector2<T> Vector2<T>::operator/(T scale) const {
	T f = T(1) / scale;
	return Vector2<T>(x * f, y * f);
}

template<class T>
Vector2<T> &Vector2<T>::operator+=(const Vector2<T> &v) {
	x += v.x; y += v.y;
	return *this;
}

template<class T>
Vector2<T> &Vector2<T>::operator-=(const Vector2<T> &v) {
	x -= v.x; y -= v.y;
	return *this;
}

template<class T>
Vector2<T> &Vector2<T>::operator*=(T scale) {
	x *= scale; y *= scale;
	return *this;
}

template<class T>
Vector2<T> &Vector2<T>::operator/=(T scale) {
	x /= scale; y /= scale;
	return *this;
}

#pragma endregion
#pragma region Vector3

template<class T>
T Vector3<T>::Length() const {
	return sqrt(x*x + y*y + z*z);
}

template<class T>
T Vector3<T>::LengthSquared() const {
	return x*x + y*y + z*z;
}

template<class T>
void Vector3<T>::Normalize() {
	T f = T(1) / Length();
	x *= f; y *= f; z *= f;
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
	T f = T(1) / scale;
	return Vector3<T>(x * f, y * f, z * f);
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
T Vector4<T>::Length() const {
	return sqrt(x*x + y*y + z*z + w*w);
}

template<class T>
T Vector4<T>::LengthSquared() const {
	return x*x + y*y + z*z + w*w;
}

template<class T>
void Vector4<T>::Normalize() {
	T f = T(1) / Length();
	x *= f; y *= f; z *= f; w *= f;
}

template<class T>
void Vector4<T>::Cartesian() {
	T f = T(1) / w;
	x *= f; y *= f; z *= f; w = T(1);
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
Vector4<T> Vector4<T>::operator+(const Vector4<T> &v) const {
	return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
}

template<class T>
Vector4<T> Vector4<T>::operator-(const Vector4<T> &v) const {
	return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
}

template<class T>
Vector4<T> Vector4<T>::operator-() const {
	return Vector4<T>(-x, -y, -z, -w);
}

template<class T>
Vector4<T> Vector4<T>::operator*(T scale) const {
	return Vector4<T>(x * scale, y * scale, z * scale, w * scale);
}

template<class T>
Vector4<T> operator*(T scale, const Vector4<T> &v) {
	return v*scale;
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
Vector4<T> Vector4<T>::operator/(T scale) const {
	T f = T(1) / scale;
	return Vector4<T>(x * f, y * f, z * f, w * f);
}

template<class T>
Vector4<T> &Vector4<T>::operator+=(const Vector4<T> &v) {
	x += v.x; y += v.y; z += v.z; w += v.w;
	return *this;
}

template<class T>
Vector4<T> &Vector4<T>::operator-=(const Vector4<T> &v) {
	x -= v.x; y -= v.y; z -= v.z; w -= v.w;
	return *this;
}

template<class T>
Vector4<T> &Vector4<T>::operator*=(T scale) {
	x *= scale; y *= scale; z *= scale; w *= scale;
	return *this;
}

template<class T>
Vector4<T> &Vector4<T>::operator*=(const Matrix44<T> &m) {
	return (*this = *this * m);
}

template<class T>
Vector4<T> &Vector4<T>::operator/=(T scale) {
	x /= scale; y /= scale; z /= scale; w /= scale;
	return *this;
}

#pragma endregion
#pragma region Matrix33

template<class T>
Matrix33<T>::Matrix33(const T m[9]) {
	memcpy(data, m, sizeof(T)*9);
}

template<class T>
template<class T2>
Matrix33<T>::Matrix33(const Matrix33<T2> &m) :
	xAxis(m.xAxis),
	yAxis(m.yAxis),
	zAxis(m.zAxis)
{ }

template<class T>
template<class T2>
Matrix33<T>::Matrix33(const Matrix44<T2> &m) :
	xAxis(m.xAxis),
	yAxis(m.yAxis),
	zAxis(m.zAxis)
{ }

template<class T>
void Matrix33<T>::Scale(T factor) {
	xAxis *= factor;
	yAxis *= factor;
	zAxis *= factor;
}

template<class T>
void Matrix33<T>::Scale(const Vector3<T> &factor) {
	xAxis *= factor.x;
	yAxis *= factor.y;
	zAxis *= factor.z;
}

template<class T>
Matrix33<T> Matrix33<T>::GetTranspose() const
{
	Matrix33<T> m1;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) m1.m[i][j] = m[j][i];
	return m1;
}

#define _DET2(i1, i2, i3, i4) (data[i1]*data[i2]-data[i3]*data[i4])
#define _DET3(i1, i2, i3, i4, i5, i6, i7, i8, i9) \
	(data[i1]*_DET2(i5,i9,i6,i8) - data[i2]*_DET2(i4,i9,i6,i7) + data[i3]*_DET2(i4,i8,i5,i7))

template<class T>
Matrix33<T> Matrix33<T>::GetInverse() const
{
	T f = T(1) / Determinant();
	T adj[9] = {
		_DET2(4,8,5,7)*f, _DET2(2,7,1,8)*f, _DET2(1,5,2,4)*f,
		_DET2(5,6,3,8)*f, _DET2(0,8,2,6)*f, _DET2(2,3,0,5)*f,
		_DET2(3,7,4,6)*f, _DET2(1,6,0,7)*f, _DET2(0,4,1,3)*f
	};
	return Matrix33<T>(adj);
}

template<class T>
T Matrix33<T>::Determinant() const
{
	return _DET3(0,1,2,3,4,5,6,7,8);
}

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
template<class T2>
Matrix44<T>::Matrix44(const Matrix33<T2> &m) :
	xAxis(m.xAxis),
	yAxis(m.yAxis),
	zAxis(m.zAxis)
{
	wx = wy = wz = T(0); wt = T(1);
}

template<class T>
template<class T2>
Matrix44<T>::Matrix44(const Matrix44<T2> &m) {
	for (int i = 0; i < 16; i++)
		data[i] = T(m.data[i]);
}

template<class T>
void Matrix44<T>::Scale(T factor) {
	xAxis *= factor;
	yAxis *= factor;
	zAxis *= factor;
}

template<class T>
void Matrix44<T>::Scale(const Vector3<T> &factor) {
	xAxis *= factor.x;
	yAxis *= factor.y;
	zAxis *= factor.z;
}

template<class T>
void Matrix44<T>::SetRotation(const Matrix33<T> &m)
{
	xAxis = m.xAxis;
	yAxis = m.yAxis;
	zAxis = m.zAxis;
}

template<class T>
Matrix44<T> Matrix44<T>::GetTranspose() const
{
	Matrix44<T> m1;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) m1.m[i][j] = m[j][i];
	return m1;
}

template<class T>
Matrix44<T> Matrix44<T>::GetInverse() const
{
	T f = T(1) / Determinant();
	T adj[16] =
	{
		 _DET3(5,6,7,9,10,11,13,14,15)*f, -_DET3(4,6,7,8,10,11,12,14,15)*f,  _DET3(4,5,7,8,9,11,12,13,15)*f, -_DET3(4,5,6,8,9,10,12,13,14)*f,
		-_DET3(1,2,3,9,10,11,13,14,15)*f,  _DET3(0,2,3,8,10,11,12,14,15)*f, -_DET3(0,1,3,8,9,11,12,13,15)*f,  _DET3(0,1,2,8,9,10,12,13,14)*f,
		 _DET3(1,2,3,5,6,7,13,14,15)*f,   -_DET3(0,2,3,4,6,7,12,14,15)*f,    _DET3(0,1,3,4,5,7,12,13,15)*f,  -_DET3(0,1,2,4,5,6,12,13,14)*f,
		-_DET3(1,2,3,5,6,7,9,10,11)*f,     _DET3(0,2,3,4,6,7,8,10,11)*f,    -_DET3(0,1,3,4,5,7,8,9,11)*f,     _DET3(0,1,2,4,5,6,8,9,10)*f
	};
	Matrix44<T> m = Matrix44<T>(adj).GetTranspose();
	return m;
}

#undef _DET2
#undef _DET3

template<class T>
T Matrix44<T>::Determinant() const
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
#pragma region Color3

template<class T>
bool Color3<T>::operator==(const Color3<T> &c) const {
	return CmpReal(r, c.r) && CmpReal(g, c.g) && CmpReal(b, c.b);
}

template<class T>
bool Color3<T>::operator!=(const Color3<T> &c) const {
	return !operator==(c);
}

template<class T>
Color3<T> operator*(T scale, Color3<T> c) {
	return Color3<T>(c.r*scale, c.g*scale, c.b*scale);
}

template<class T>
Color3<T> Color3<T>::operator*(T scale) const {
	return Color3<T>(r*scale, g*scale, b*scale);
}

template<class T>
Color3<T> Color3<T>::operator/(T scale) const {
	T f = T(1) / scale;
	return Color3<T>(r*f, g*f, b*f);
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

template<class T>
Color4<T> operator*(T scale, Color4<T> c) {
	return Color4<T>(c.r*scale, c.g*scale, c.b*scale, c.a*scale);
}

template<class T>
Color4<T> Color4<T>::operator*(T scale) const {
	return Color4<T>(r*scale, g*scale, b*scale, c.a*scale);
}

template<class T>
Color4<T> Color4<T>::operator/(T scale) const {
	T f = T(1) / scale;
	return Color4<T>(r*f, g*f, b*f, a*f);
}
#pragma endregion
#endif // _DATATYPES_INL_
