#include "frustumculler.h"
#include "glcontext.h"

void FrustumCuller::ComputePlanes()
{
	Matrix44f mvp = rc->GetProjection() * rc->GetModelViewRef();
	float(&m)[4][4] = mvp.m;

	/*planes[0] = Plane(
		m[0][0] + m[3][0],
		m[0][1] + m[3][1],
		m[0][2] + m[3][2],
		m[0][3] + m[3][3]);
	planes[0].Normalize();

	planes[1] = Plane(
		m[3][0] - m[0][0],
		m[3][1] - m[0][1],
		m[3][2] - m[0][2],
		m[3][3] - m[0][3]);
	planes[1].Normalize();

	planes[2] = Plane(
		m[1][0] + m[3][0],
		m[1][1] + m[3][1],
		m[1][2] + m[3][2],
		m[1][3] + m[3][3]);
	planes[2].Normalize();

	planes[3] = Plane(
		m[3][0] - m[1][0],
		m[3][1] - m[1][1],
		m[3][2] - m[1][2],
		m[3][3] - m[1][3]);
	planes[3].Normalize();

	planes[4] = Plane(
		m[2][0] + m[3][0],
		m[2][1] + m[3][1],
		m[2][2] + m[3][2],
		m[2][3] + m[3][3]);
	planes[4].Normalize();

	planes[5] = Plane(
		m[3][0] - m[2][0],
		m[3][1] - m[2][1],
		m[3][2] - m[2][2],
		m[3][3] - m[2][3]);
	planes[5].Normalize();*/

	planes[0] = Plane(
		m[0][0] + m[0][3],
		m[1][0] + m[1][3],
		m[2][0] + m[2][3],
		m[3][0] + m[3][3]);
	planes[0].Normalize();

	planes[1] = Plane(
		m[0][3] - m[0][0],
		m[1][3] - m[1][0],
		m[2][3] - m[2][0],
		m[3][3] - m[3][0]);
	planes[1].Normalize();

	planes[2] = Plane(
		m[0][1] + m[0][3],
		m[1][1] + m[1][3],
		m[2][1] + m[2][3],
		m[3][1] + m[3][3]);
	planes[2].Normalize();

	planes[3] = Plane(
		m[0][3] - m[0][1],
		m[1][3] - m[1][1],
		m[2][3] - m[2][1],
		m[3][3] - m[3][1]);
	planes[3].Normalize();

	planes[4] = Plane(
		m[0][2] + m[0][3],
		m[1][2] + m[1][3],
		m[2][2] + m[2][3],
		m[3][2] + m[3][3]);
	planes[4].Normalize();

	planes[5] = Plane(
		m[0][3] - m[0][2],
		m[1][3] - m[1][2],
		m[2][3] - m[2][2],
		m[3][3] - m[3][2]);
	planes[5].Normalize();

	fComputePlanes = false;
}