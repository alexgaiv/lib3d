#ifndef _MODEL_H_
#define _MODEL_H_

#include "common.h"
#include "nullable.h"
#include "quaternion.h"
#include "shader.h"
#include "mesh.h"
#include "glcontext.h"

class Model
{
public:
	Vector3f location;
	Quaternion rotation;
	Vector3f scale;
	vector<Mesh> meshes;
	Nullable<ProgramObject> shader;

	Model(GLRenderingContext *rc);

	Matrix44f GetTransform() const { return transform; }
	const Matrix44f &GetTransformRef() const { return transform; }

	void AddMesh(const Mesh &mesh) { meshes.push_back(mesh); }
	bool LoadObj(const char *filename);
	bool LoadRaw(const char *filename);
	void UpdateTransform();
	void ApplyTransform();
	void Draw();
private:
	GLRenderingContext *rc;
	Matrix44f transform;
};

#endif // _MODEL_H_