#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "common.h"
#include "quaternion.h"
#include "mesh.h"
#include "global.h"

class Actor
{
public:
	Vector3f location;
	Quaternion rotation;
	float scale;
	Matrix44f transform;
	Mesh mesh;

	Actor() : scale(1.0f) { }

	void ApplyTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		if (scale != 1.0f) transform.Scale(scale);
		Global::MultModelView(transform);
	}

	void Draw() {
		Global::PushModelView();
		ApplyTransform();
		mesh.Draw();
		Global::PopModelView();
	}
};

#endif // _ACTOR_H_