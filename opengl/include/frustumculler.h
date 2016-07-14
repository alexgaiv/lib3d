#ifndef _FRUSTUM_CULLER_H_
#define _FRUSTUM_CULLER_H_

#include "geometry.h"

class GLRenderingContext;

class FrustumCuller
{
public:
	FrustumCuller(GLRenderingContext *rc) : rc(rc), fComputePlanes(true)
	{ }

	bool Cull(const AABox &boundingBox)
	{
		if (fComputePlanes) ComputePlanes();

		for (int i = 0; i < 6; i++)
		{
			Vector3f normal = planes[i].Normal();
			Point3f p = boundingBox.vmin;

			if (normal.x >= 0) p.x = boundingBox.vmax.x;
			if (normal.y >= 0) p.y = boundingBox.vmax.y;
			if (normal.z >= 0) p.z = boundingBox.vmax.z;

			if (planes[i].Distance(p) < 0)
				return false;
		}

		return true;
	}

	void UpdateMVP() {
		fComputePlanes = true;
	}
private:
	GLRenderingContext *rc;
	Plane planes[6];
	bool fComputePlanes;
	void ComputePlanes();
};

#endif // _FRUSTUM_CULLER_H_