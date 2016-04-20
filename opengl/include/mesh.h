#ifndef _MESH_H_
#define _MESH_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "texture.h"
#include "glcontext.h"
#include "datatypes.h"
#include "vertexbuffer.h"
#include "shader.h"
#include "geometry.h"

using namespace std;

class BoundingBox
{
public:
	Plane front, back, top, bottom, left, right;

	bool Intersects(const Ray &r)
	{
		Point3f p;
		if (front.Intersects(r, p)) {
			if (p.x > left.D && p.x < -right.D &&
				p.y > bottom.D && p.y < -top.D) return true;
		}
		if (back.Intersects(r, p)) {
			if (p.x > left.D && p.x < -right.D &&
				p.y > bottom.D && p.y < -top.D) return true;
		}
		if (top.Intersects(r, p)) {
			if (p.x > left.D && p.x < -right.D &&
				p.z > back.D && p.z < -front.D) return true;
		}
		if (bottom.Intersects(r, p)) {
			if (p.x > left.D && p.x < -right.D &&
				p.z > back.D && p.z < -front.D) return true;
		}
		if (left.Intersects(r, p)) {
			if (p.y > bottom.D && p.y < -top.D &&
				p.z > back.D && p.z < -front.D) return true;
		}
		if (right.Intersects(r, p)) {
			if (p.y > bottom.D && p.y < -top.D &&
				p.z > back.D && p.z < -front.D) return true;
		}

		return false;
	}
};

class Mesh
{
public:
	Mesh(GLRenderingContext *rc);
	Mesh(const Mesh &m);
	~Mesh();

	Mesh &operator=(const Mesh &m);

	bool HasNormals() const { return normals != NULL; }
	bool HasTexCoords() const { return texCoords != NULL; }
	int GetVerticesCount() const { return vertices->GetSize() / sizeof(Vector3f); }
	int GetIndicesCount() const { return indicesCount >= 0 ? indicesCount : indices->GetSize() / sizeof(int); }
	int GetFaceCount() const { return GetIndicesCount() / 3; }

	void SetFirstIndex(int firstIndex) { this->firstIndex = firstIndex; }
	void SetIndicesCount(int indicesCount) { this->indicesCount = indicesCount; } // -1 to draw all

	void BindTexture(const BaseTexture &texture);
	void BindNormalMap(const Texture2D &normalMap);
	void BindSpecularMap(const Texture2D &specularMap);

	void RecalcTangents();

	void Draw();
	void DrawInstanced(int instanceCount);
	void DrawFixed();
	bool LoadObj(const char *filename);
	bool LoadRaw(const char *filename);

	BoundingBox boundingBox;
	Sphere boundingSphere;

	VertexBuffer *vertices;
	VertexBuffer *indices;
	VertexBuffer *normals;
	VertexBuffer *texCoords;
	VertexBuffer *tangents, *binormals;
private:
	GLRenderingContext *rc;
	BaseTexture *texture;
	Texture2D *normalMap, *specularMap;

	int firstIndex;
	int indicesCount;
	bool tangentsComputed;

	void clone(const Mesh &m);
	void cleanup();

	void read_num(const string &line, char &c, int &i, int &n);
};

#endif // _MESH_H_