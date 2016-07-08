#ifndef _QSLERP_H_
#define _QSLERP_H_

#include <math.h>
#include "quaternion.h"

class QSlerp
{
public:
	QSlerp();
	QSlerp(const Quaternion &from, const Quaternion &to, float step);

	void Setup(const Quaternion &from, const Quaternion &to, float step);
	Quaternion Next();

	Quaternion GetStartState() { return q1; }
	Quaternion GetEndState()   { return q2; }
	float GetStep() { return step; }
	bool IsComplete() { return is_end; }
private:
	Quaternion q1, q2;
	float omega, s;
	float t, step;
	bool is_end;
};


inline QSlerp::QSlerp()
{
	Quaternion q = Quaternion::Identity();
	this->Setup(q, q, 0.0f);
	is_end = true;
}

inline QSlerp::QSlerp(const Quaternion &from, const Quaternion &to, float step) {
	this->Setup(from, to, step);
}

inline void QSlerp::Setup(const Quaternion &from, const Quaternion &to, float step)
{
	float dot = from.x*to.x + from.y*to.y + from.z*to.z + from.w*to.w;

	if (dot < 0.0f) {
		dot = -dot;
		q2 = -to;
	} else q2 = to;

	q1 = from;
	omega = acos(dot);
	s = 1.0f / sin(omega);

	q1.Normalize();
	q2.Normalize();

	t = 0.0f;
	this->step = step;
	is_end = false;
}

inline Quaternion QSlerp::Next()
{
	if (t > 1.0f) {
		is_end = true;
		return q2;
	}

	Quaternion q;
	if (omega > 0.00001) {
		float s1 = sin((1 - t) * omega) * s;
		float s2 = sin(t * omega) * s;
	
		q = Quaternion(
			q1.x * s1 + q2.x * s2,
			q1.y * s1 + q2.y * s2,
			q1.z * s1 + q2.z * s2,
			q1.w * s1 + q2.w * s2);
	}
	else {
		float u = 1 - t;
		q = Quaternion(
			q1.x * u + q2.x * t,
			q1.y * u + q2.y * t,
			q1.z * u + q2.z * t,
			q1.w * u + q2.w * t);
	}

	t += step;
	return q;
}

#endif // _QSLERP_H_