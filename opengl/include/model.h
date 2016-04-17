#ifndef _MODEL_H_
#define _MODEL_H_

#include "common.h"
#include "quaternion.h"
#include "mesh.h"
#include "shader.h"

class Model
{
public:
	Vector3f location;
	Quaternion rotation;
	Vector3f scale;
	vector<Mesh *> meshes;

	Model(GLRenderingContext *rc);
	Model(const Model &m);
	~Model();

	Model &operator=(const Model &m);

	void AddMesh(const Mesh &mesh) { meshes.push_back(new Mesh(mesh)); }
	bool LoadObj(const char *filename);

	Matrix44f GetTransform() const { return transform; }
	const Matrix44f &GetTransformRef() const { return transform; }

	void BindShader(const ProgramObject &program) {
		delete this->program;
		this->program = new ProgramObject(program);
	}

	void UpdateTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
			transform.Scale(scale);
	}

	void ApplyTransform() {
		UpdateTransform();
		rc->MultModelView(transform);
	}

	void Draw()
	{
		if (program) program->Use();
		rc->PushModelView();
		ApplyTransform();
		for (int i = 0, s = meshes.size(); i < s; i++)
			meshes[i]->Draw();
		rc->PopModelView();
	}
private:
	GLRenderingContext *rc;
	Matrix44f transform;
	ProgramObject *program;

	void clone(const Model &m);
	void cleanup();
	void read_num(const string &line, char &c, int &i, int &n);
};

#endif // _MODEL_H_