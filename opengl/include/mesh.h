#ifndef _MESH_H_
#define _MESH_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common.h"
#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

using namespace std;

class Mesh
{
public:
	Mesh();
	void BindTexture(const BaseTexture &texture);
	void BindShader(const ProgramObject &program);

	void Draw(int firstIndex = 0, int numIndices = -1);
	void DrawFixed(int firstIndex = 0, int numIndices = -1);
	bool LoadObj(const char *filename);
	bool LoadRaw(const char *filename);

	bool HasNormals() const { return hasNormals; }
	bool HasTexCoords() const { return hasTexCoords; }
	int GetVerticesCount() const { return verticesCount; }
	int GetIndicesCount() const { return indicesCount; }
	int GetFaceCount() const { return indicesCount / 3; }

	VertexBuffer vertices;
	VertexBuffer indices;
	VertexBuffer normals;
	VertexBuffer texCoords;
private:
	BaseTexture texture;
	GLuint programId;

	bool hasNormals;
	bool hasTexCoords;
	int verticesCount;
	int indicesCount;

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