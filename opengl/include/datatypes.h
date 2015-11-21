#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include "common.h"

template<class T>
inline bool CmpReal(T a, T b, T eps = T(0.0001)) {
	return (T)fabs(double(a - b)) <= eps;
}

template<class T>
union Point2
{
	T data[2];
	struct {
		T x, y;
	};

	Point2() { x = y = T(0); }
	Point2(T x, T y) {
		this->x = x;
		this->y = y;
	}

	bool operator==(const Point2<T> &p) const;
	bool operator!=(const Point2<T> &p) const;
};

template<class T>
union Point3
{
	T data[3];
	struct {
		T x, y, z;
	};

	Point3() { x = y = z = T(0); }
	Point3(T x, T y, T z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const Point3<T> &p) const;
	bool operator!=(const Point3<T> &p) const;
};

template<class T> union Vector4;
template<class T> union Matrix33;
template<class T> union Matrix44;

template<class T>
union Vector3
{
	T data[3];
	struct {
		T x, y, z;
	};

	Vector3() { x = y = z = T(0); }
	Vector3(T x, T y, T z) {
		this->x = x; this->y = y; this->z = z;
	}
	explicit Vector3(const Vector4<T> &v) {
		x = v.x; y = v.y; z = v.z;
	}
	template<class T2> explicit Vector3(const Vector3<T2> &v) {
		x = T(v.x); y = T(v.y); z = T(v.z);
	}

	T Length() const;
	T LengthSquared() const;
	void Normalize();

	static T Dot(Vector3<T> v1, Vector3<T> v2);
	static Vector3<T> Cross(Vector3<T> v1, Vector3<T> v2);

	T &operator[](int i) { return data[i]; }
	const T &operator[](int i) const { return data[i]; }

	bool operator==(const Vector3<T> &v) const;
	bool operator!=(const Vector3<T> &v) const;
	Vector3<T> operator+(const Vector3<T> &v) const;
	Vector3<T> operator-(const Vector3<T> &v) const;
	Vector3<T> operator-() const;
	Vector3<T> operator*(T scale) const;
	Vector3<T> operator*(const Matrix33<T> &m) const;
	Vector3<T> operator/(T scale) const;

	Vector3<T> &operator+=(const Vector3<T> &v);
	Vector3<T> &operator-=(const Vector3<T> &v);
	Vector3<T> &operator*=(T scale);
	Vector3<T> &operator*=(const Matrix33<T> &m);
	Vector3<T> &operator/=(T scale);
};

template<class T>
union Vector4
{
	T data[4];
	struct {
		T x, y, z, w;
	};

	Vector4() { x = y = z = T(0); w = T(1); }
	Vector4(T x, T y, T z, T w = T(1)) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
	explicit Vector4(const Vector3<T> &v) {
		x = v.x; y = v.y; z = v.z; w = T(1);
	}
	template<class T2> explicit Vector4(const Vector4<T2> &v) {
		x = T(v.x); y = T(v.y); z = T(v.z); w = T(v.w);
	}

	void Cartesian();

	T &operator[](int i) { return data[i]; }
	const T &operator[](int i) const { return data[i]; }

	bool operator==(const Vector4<T> &v) const;
	bool operator!=(const Vector4<T> &v) const;
	Vector4<T> operator*(const Matrix44<T> &m) const;
	Vector4<T> &operator*=(const Matrix44<T> &m);
};

template<class T>
union Matrix33
{
	T data[9];
	T m[3][3];

	struct {
		Vector3<T> xAxis, yAxis, zAxis;
	};

	Matrix33() {
		data[0] = data[4] = data[8] = T(1);
	}
	Matrix33(T diag) {
		data[0] = data[4] = data[8] = diag;
	}
	Matrix33(const T m[9]);
	explicit Matrix33(const Matrix44<T> &m);
	template<class T2> explicit Matrix33(const Matrix33<T2> &m);

	void Scale(T factor);
	T Determinant() const;
	bool GetInverse(Matrix33<T> &out) const;
	Matrix33<T> GetTranspose() const;
	
	void LoadIdentity();
	static Matrix33<T> Identity();
	static Matrix33<T> Multiply(const Matrix33<T> &m1, const Matrix33<T> &m2);
	static Matrix33<T> Multiply(const Matrix33<T> &m, const Vector3<T> &v);

	bool operator==(const Matrix33<T> &m) const;
	bool operator!=(const Matrix33<T> &m) const;
	Vector3<T> operator*(const Vector3<T> &v) const;
	Matrix33<T> operator*(const Matrix33<T> &m) const;
	Matrix33<T> &operator*=(const Matrix33<T> &m);
};

template<class T>
union Matrix44
{
	T data[16];
	T m[4][4];

	struct {
		Vector3<T> xAxis; T wx;
		Vector3<T> yAxis; T wy;
		Vector3<T> zAxis; T wz;
		Vector3<T> translate; T wt;
	};

	Matrix44() {
		wx = wy = wz = T(0);
		data[0] = data[5] = data[10] = data[15] = T(1);
	}
	Matrix44(T diag) {
		wx = wy = wz = T(0);
		data[0] = data[5] = data[10] = data[15] = diag;
	}
	Matrix44(const T m[16]);
	explicit Matrix44(const Matrix33<T> &m);
	template<class T2> explicit Matrix44(const Matrix44<T2> &m);

	void Scale(T factor);
	void SetRotation(const Matrix33<T> &m);

	T Determinant() const;
	Matrix44<T> GetTranspose() const;
	bool GetInverse(Matrix44<T> &out) const;

	void LoadIdentity();
	static Matrix44<T> Identity();
	static Matrix44<T> Multiply(const Matrix44<T> &m1, const Matrix44<T> &m2);

	bool operator==(const Matrix44<T> &m) const;
	bool operator!=(const Matrix44<T> &m) const;
	Vector4<T> operator*(const Vector4<T> &v) const;
	Matrix44<T> operator*(const Matrix44<T> &m) const;
	Matrix44<T> &operator*=(const Matrix44<T> &m);
};

template<class T>
union Color4
{
	T data[4];
	struct {
		T r, g, b, a;
	};

	Color4() { r = g = b = a = T(0); }
	Color4(T r, T g, T b, T a = T(1)) {
		this->r = r; this->g = g; this->b = b; this->a = a;
	}

	bool operator==(const Color4<T> &c) const;
	bool operator!=(const Color4<T> &c) const;
};

template<class T>
union Color3
{
	T data[3];
	struct {
		T r, g, b;
	};

	Color3() { r = g = b = T(0); }
	Color3(T r, T g, T b, T a = T(1)) {
		this->r = r; this->g = g; this->b = b;
	}

	bool operator==(const Color3<T> &c) const;
	bool operator!=(const Color3<T> &c) const;
};

typedef Point2<int>      Point2i;
typedef Point2<float>    Point2f;
typedef Point3<float>    Point3f;
typedef Vector3<float>   Vector3f;
typedef Vector4<float>   Vector4f;
typedef Matrix33<float>  Matrix33f;
typedef Matrix44<float>  Matrix44f;
typedef Color4<float>    Color4f;
typedef Color3<float>    Color3f;

typedef Point2<double>   Point2d;
typedef Point3<double>   Point3d;
typedef Vector3<double>  Vector3d;
typedef Vector4<double>  Vector4d;
typedef Matrix33<double> Matrix33d;
typedef Matrix44<double> Matrix44d;
typedef Color4<double>   Color4d;

typedef Color4<unsigned char> Color4b;
typedef Color3<unsigned char> Color3b;

#include "datatypes.inl"

#endif // _DATATYPES_H_