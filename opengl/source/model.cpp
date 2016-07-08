#include "model.h"
#include "modelloader.h"

Model::Model(GLRenderingContext *rc)
	: rc(rc), scale(Vector3f(1.0f)) { }

bool Model::LoadObj(const char *filename)
{
	meshes.clear();
	ModelLoader loader(rc);
	return loader.LoadObj(filename, meshes);
}

bool Model::LoadRaw(const char *filename)
{
	meshes.clear();
	ModelLoader loader(rc);
	return loader.LoadRaw(filename, meshes);
}

void Model::UpdateTransform() {
	rotation.ToMatrix(transform);
	transform.translate = location;
	if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		transform.Scale(scale);
}

void Model::ApplyTransform() {
	UpdateTransform();
	rc->MultModelView(transform);
}

void Model::Draw()
{
	if (shader) shader->Use();
	rc->PushModelView();
	ApplyTransform();
	for (int i = 0, s = meshes.size(); i < s; i++)
		meshes[i].Draw();
	rc->PopModelView();
}