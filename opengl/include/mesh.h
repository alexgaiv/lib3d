#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "nullable.h"
#include "datatypes.h"
#include "geometry.h"
#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#include "glcontext.h"

using namespace std;

enum VertexFormat
{
	VF_XYZ = 1,
	VF_NORMAL = 2,
	VF_TEXCOORD = 4,
	VF_TANGENTS_BINORMALS = 8
};

class Mesh
{
public:
	Mesh(GLRenderingContext *rc);

	bool HasNormals() const { return normals != NULL; }
	bool HasTexCoords() const { return texCoords != NULL; }
	int GetVertexCount() const { return vertices->GetSize() / sizeof(Vector3f); }
	int GetIndexCount() const { return numIndices >= 0 ? numIndices : indices->GetSize() / sizeof(int); }
	int GetFaceCount() const { return GetIndexCount() / 3; }

	void SetVertexFormat(int vfFlags);
	void SetFirstIndex(int firstIndex) { this->firstIndex = firstIndex; }
	void SetIndexCount(int numIndices) { this->numIndices = numIndices; } // -1 to draw all

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
	int numIndices;
	//int vertexFormat;
};

#endif // _MESH_H_