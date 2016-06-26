#include "model.h"
#include "modelloader.h"

Model::Model(GLRenderingContext *rc)
	: rc(rc), scale(Vector3f(1.0f)), program(NULL) { }

Model::Model(const Model &m) {
	clone(m);
}

Model::~Model() {
	cleanup();
}

Model &Model::operator=(const Model &m) {
	cleanup();
	clone(m);
	return *this;
}

void Model::clone(const Model &m)
{
	rc = m.rc;
	transform = m.transform;
	location = m.location;
	rotation = m.rotation;
	scale = m.scale;
	program = m.program ? new ProgramObject(*m.program) : NULL;

	int s = m.meshes.size();
	meshes.reserve(s);
	for (int i = 0; i < s; i++)
		meshes.push_back(new Mesh(*m.meshes[i]));
}

void Model::cleanup() {
	delete program;
	cleanupMeshes();
}

void Model::cleanupMeshes() {
	for (int i = 0, s = meshes.size(); i < s; i++)
		delete meshes[i];
}

bool Model::LoadObj(const char *filename)
{
	cleanupMeshes();
	meshes.clear();
	ModelLoader loader(rc);
	return loader.LoadObj(filename, meshes);
}

bool Model::LoadRaw(const char *filename)
{
	cleanupMeshes();
	meshes.clear();
	ModelLoader loader(rc);
	return loader.LoadRaw(filename, meshes);
}