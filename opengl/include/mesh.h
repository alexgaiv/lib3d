#ifndef _MESH_H_
#define _MESH_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "nullable.h"
#include "texture.h"
#include "glcontext.h"
#include "datatypes.h"
#include "vertexbuffer.h"
#include "shader.h"
#include "geometry.h"

using namespace std;

enum VertexFormat
{
	VF_XYZ = 1,
	VF_NORMAL = 2,
	VF_TEXCOORD = 4
};

class Mesh
{
public:
	Mesh(GLRenderingContext *rc);

	bool HasNormals() const { return normals != NULL; }
	bool HasTexCoords() const { return texCoords != NULL; }
	int GetVerticesCount() const { return vertices->GetSize() / sizeof(Vector3f); }
	int GetIndicesCount() const { return indicesCount >= 0 ? indicesCount : indices->GetSize() / sizeof(int); }
	int GetFaceCount() const { return GetIndicesCount() / 3; }

	void SetVertexFormat(int vfFlags);
	void SetFirstIndex(int firstIndex) { this->firstIndex = firstIndex; }
	void SetIndicesCount(int indicesCount) { this->indicesCount = indicesCount; } // -1 to draw all

	void RecalcTangents();

	void Draw();
	void DrawInstanced(int instanceCount);
	void DrawFixed();
	bool LoadObj(const char *filename);
	bool LoadRaw(const char *filename);

	AABox boundingBox;
	Sphere boundingSphere;

	VertexArrayObject vao;
	Material material;
	Nullable<VertexBuffer> vertices;
	Nullable<VertexBuffer> indices;
	Nullable<VertexBuffer> normals;
	Nullable<VertexBuffer> texCoords;
	Nullable<VertexBuffer> tangents, binormals;
private:
	GLRenderingContext *rc;

	int firstIndex;
	int indicesCount;
	bool tangentsComputed;
};

#endif // _MESH_H_