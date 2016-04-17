#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "datatypes.h"

class Plane;
class Sphere;

class Line
{
public:
	Point3f p;
	Vector3f v;

	Line() { }
	Line(const Point3f &p, Vector3f &v) : p(p), v(v) {
		v.Normalize();
	}

	float Distance(const Point3f &p) const
	{
		Vector3f u = p - this->p;
		Vector3f pu = Dot(u, v) * v;
		return (u - pu).Length();
	}
};

class Ray
{
public:
	Point3f p;
	Vector3f v;

	Ray() { }
	Ray(const Point3f &p, Vector3f &v) : p(p), v(v) {
		v.Normalize();
	}

	float Distance(const Point3f &p) const
	{
		Vector3f u = p - this->p;
		if (Dot(u, v) < 0)
			return u.Length();

		Vector3f pu = Dot(u, v) * v;
		return (p - (this->p + pu)).Length();
	}
};

class Plane
{
public:
	float A, B, C, D;

	Plane() { A = B = C = D; }
	Plane(float A, float B, float C, float D) : A(A), B(B), C(C), D(D) { }

	Plane(const Point3f &p1, const Point3f &p2, const Point3f &p3)
	{
		Vector3f n = Normalize(Cross(p2 - p1, p3 - p1));
		A = n.x;
		B = n.y;
		C = n.z;
		D = -Dot(n, p1);
	}

	float GetX(float y, float z) const {
		return (-B*y - C*z - D) / A;
	}

	float GetY(float x, float z) const {
		return (-A*x - C*z - D) / B;
	}

	float GetZ(float x, float y) const {
		return (-A*x - B*y - D) / C;
	}

	Vector3f Normal() const {
		return Vector3f(A, B, C);
	}

	float Distance(const Point3f &p) const {
		return A*p.x + B*p.y + C*p.z + D;
	}

	Point3f Project(const Point3f &p) const {
		return p - Distance(p) * Normal();
	}

	bool Intersects(const Line &l, Point3f &intersection) const
	{
		Vector3f n(A, B, C);
		float d = Dot(l.v, n);
		if (CmpReal(d, 0.0f)) return false;
		float t = -(D + Dot(l.p, n)) / d;
		intersection = l.p + l.v * t;
		return true;
	}

	bool Intersects(const Line &l) const
	{
		Vector3f n(A, B, C);
		return !CmpReal(Dot(l.v, n), 0.0f);
	}

	bool Intersects(const Ray &r, Point3f &intersection) const {
		Vector3f n(A, B, C);
		float d = Dot(r.v, n);
		if (CmpReal(d, 0.0f)) return false;
		float t = -(D + Dot(r.p, n)) / d;
		if (t < 0) return false;
		intersection = r.p + r.v * t;
		return true;
	}

	bool Intersects(const Ray &r) const {
		Vector3f n(A, B, C);
		float d = Dot(r.v, n);
		if (CmpReal(d, 0.0f)) return false;
		return -(D + Dot(r.p, n)) / d >= 0;
	}
};

class Sphere
{
public:
	Point3f center;
	float radius;

	Sphere() { radius = 0; }
	Sphere(const Point3f &center, float radius)
		: center(center), radius(radius) { }

	bool Intersects(const Ray &r) const
	{
		Vector3f u = center - r.p;
		if (Dot(u, r.v) < 0) {
			return u.Length() <= radius;
		}
		else {
			Vector3f pu = Dot(u, r.v) * r.v;
			float dist = (u - pu).Length();
			return dist <= radius;
		}
	}
};

#endif // _GEOMETRY_H_