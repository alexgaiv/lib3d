#ifndef _MESH_H_
#define _MESH_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "texture.h"
#include "glcontext.h"
#include "vertexbuffer.h"
#include "shader.h"

using namespace std;

class Mesh
{
public:
	Mesh(GLRenderingContext *rc);
	Mesh(const Mesh &m);
	~Mesh();

	Mesh &operator=(const Mesh &m);

	bool HasNormals() const { return hasNormals; }
	bool HasTexCoords() const { return hasTexCoords; }
	int GetVerticesCount() const { return verticesCount; }
	int GetIndicesCount() const { return indicesCount; }
	int GetFaceCount() const { return indicesCount / 3; }

	void BindTexture(const BaseTexture &texture);
	void BindNormalMap(const Texture2D &normalMap);
	void BindSpecularMap(const Texture2D &specMap);
	void BindShader(const ProgramObject &program);

	void RecalcTangents();

	void Draw(int firstIndex = 0, int numIndices = -1);
	void DrawFixed(int firstIndex = 0, int numIndices = -1);
	bool LoadObj(const char *filename);
	bool LoadRaw(const char *filename);

	VertexBuffer vertices;
	VertexBuffer indices;
	VertexBuffer normals;
	VertexBuffer texCoords;
	VertexBuffer tangents, binormals;
private:
	GLRenderingContext *rc;
	ProgramObject *program;
	BaseTexture *texture;
	Texture2D *normalMap, *specularMap;

	bool tangentsComputed;
	bool hasNormals;
	bool hasTexCoords;
	int verticesCount;
	int indicesCount;

	void clone(const Mesh &m);
	void cleanup();

	void read_num(const string &line, char &c, int &i, int &n) {
		n = 0;
		c = line[i++];
		bool neg = c == '-';
		if (neg) c = line[i++];
		while (c != '/' && c != ' ' && c != 0) {
			n = n*10 + c-'0';
			c = line[i++];
		}
		if (neg) n = -n;
	}
};

#endif // _MESH_H_