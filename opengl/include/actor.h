#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "common.h"
#include "quaternion.h"
#include "mesh.h"

class Actor
{
public:
	Vector3f location;
	Quaternion rotation;
	float scale;
	Matrix44f transform;
	Mesh mesh;

	Actor(GLRenderingContext *rc) : rc(rc), mesh(rc), scale(1.0f) { }

	void ApplyTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		if (scale != 1.0f) transform.Scale(scale);
		rc->MultModelView(transform);
	}

	void Draw() {
		rc->PushModelView();
		ApplyTransform();
		mesh.Draw();
		rc->PopModelView();
	}
protected:
	GLRenderingContext *rc;
};

#endif // _ACTOR_H_