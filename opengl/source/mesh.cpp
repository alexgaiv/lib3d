#include "mesh.h"

Mesh::Mesh() : texture(GL_TEXTURE_2D, GLuint(-2)), textureUnit(GLenum(-1)), programId(0)
{
	verticesCount = indicesCount = 0;
	vertices.Bind(GL_ARRAY_BUFFER);
	indices.Bind(GL_ELEMENT_ARRAY_BUFFER);
	normals.Bind(GL_ARRAY_BUFFER);
	texCoords.Bind(GL_ARRAY_BUFFER);
}

void Mesh::BindTexture(const BaseTexture &texture, GLenum textureUnit) {
	this->texture = texture;
	this->textureUnit = textureUnit;
}

void Mesh::BindShader(const ProgramObject &program) {
	this->programId = program.Handle();
}

void Mesh::Draw(int first, int count)
{
	if (texture.GetId() != GLuint(-2))
		texture.Bind(textureUnit);

	if (programId && programId != Global::curProgram)
		glUseProgram(programId);

	glEnableVertexAttribArray(0);
	vertices.Bind(GL_ARRAY_BUFFER);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (hasNormals) {
		glEnableVertexAttribArray(1);
		normals.Bind(GL_ARRAY_BUFFER);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (hasTexCoords) {
		glEnableVertexAttribArray(2);
		texCoords.Bind(GL_ARRAY_BUFFER);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	indices.Bind(GL_ELEMENT_ARRAY_BUFFER);
	glDrawElements(GL_TRIANGLES, count == -1 ? indicesCount : count, GL_UNSIGNED_INT, (void *)first);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Mesh::DrawFixed(int first, int count)
{
	if (texture.GetId() != GLuint(-2))
		texture.Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	vertices.Bind(GL_ARRAY_BUFFER);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	if (hasNormals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		normals.Bind(GL_ARRAY_BUFFER);
		glNormalPointer(GL_FLOAT, 0, 0);
	}

	if (hasTexCoords) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		texCoords.Bind(GL_ARRAY_BUFFER);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	indices.Bind(GL_ELEMENT_ARRAY_BUFFER);
	glDrawElements(GL_TRIANGLES, count == -1 ? indicesCount : count, GL_UNSIGNED_INT, (void *)first);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

bool Mesh::LoadObj(const char *filename)
{
	ifstream file(filename);
	if (!file) return false;

	Vertex v = { };
	TexCoord tc = { };

	vector<Vertex> verts, norms;
	vector<TexCoord> texs;
	vector<int> iverts, inorms, itexs;

	string line;
	while (getline(file, line))
	{
		if (line.size() < 2) continue;
		string prefix = line.substr(0, 2);
		line[0] = line[1] = ' ';

		int s = 2;
		while (isspace(line[s++]));
		line = line.substr(s - 1);

		if (prefix == "v ") {
			sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			verts.push_back(v);
		}
		else if (prefix == "vn") {
			sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			norms.push_back(v);	 
		}
		else if (prefix == "vt") {
			sscanf_s(line.c_str(), "%f %f", &tc.s, &tc.t);
			texs.push_back(tc);
		}
		else if (prefix == "f ")
		{
			char c = 0;
			int i = 0;
			int n = 0;
			int vertsCount = verts.size();

			for (int k = 0; k < 3; k++) {
				read_num(line, c, i, n);
				iverts.push_back(n > 0 ? --n : vertsCount + n);

				if (c == '/')
				{
					bool skip = line[i] == '/';
					if (skip) i++;
					read_num(line, c, i, n);

					if (skip)
						inorms.push_back(n > 0 ? --n : vertsCount + n);
					else {
						itexs.push_back(n > 0 ? --n : vertsCount + n);
						if (c == '/') {
							read_num(line, c, i, n);
							inorms.push_back(n > 0 ? --n : vertsCount + n);
						}
					}
				}
			}
		}
	}

	verticesCount = verts.size();
	indicesCount = iverts.size();

	hasNormals = norms.size() != 0;
	hasTexCoords = texs.size() != 0;

	if (hasNormals || hasTexCoords)
	{
		vector<Vertex> norms_new(verts.size());
		vector<TexCoord> texs_new(verts.size());
			
		for (int i = 0, k = iverts.size(); i < k; i++) {
			int ivert = iverts[i];
			if (hasNormals)
				norms_new[ivert] = norms[inorms[i]];
			if (hasTexCoords)
				texs_new[ivert] = texs[itexs[i]];
		}

		if (hasNormals)
			normals.SetData(norms_new.size()*sizeof(Vertex), norms_new.data(), GL_STATIC_DRAW);
		if (hasTexCoords)
			texCoords.SetData(texs_new.size()*sizeof(TexCoord), texs_new.data(), GL_STATIC_DRAW);
	}
	
	vertices.SetData(verticesCount*sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	indices.SetData(indicesCount*sizeof(int), iverts.data(), GL_STATIC_DRAW);

	file.close();
	return true;
}

bool Mesh::LoadRaw(const char *filename)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	DWORD bytesRead = 0;

	BYTE signature[4] = { };
	ReadFile(hFile, signature, 3, &bytesRead, NULL);
	int a = *(int *)signature;
	if (*(int *)signature != 0x00574152) return false;

	ReadFile(hFile, &verticesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &indicesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &hasNormals, 1, &bytesRead, NULL);
	ReadFile(hFile, &hasTexCoords, 1, &bytesRead, NULL);

	Vertex *verts = new Vertex[verticesCount];
	UINT *indx = new UINT[indicesCount];

	ReadFile(hFile, verts, verticesCount*sizeof(Vertex), &bytesRead, NULL);
	ReadFile(hFile, indx, indicesCount*sizeof(UINT), &bytesRead, NULL);

	vertices.SetData(verticesCount*sizeof(Vertex), verts, GL_STATIC_DRAW);
	indices.SetData(indicesCount*sizeof(UINT), indx, GL_STATIC_DRAW);

	delete [] verts;
	delete [] indx;

	if (hasNormals)
	{
		Vertex *norms = new Vertex[verticesCount];
		ReadFile(hFile, norms, verticesCount*sizeof(Vertex), &bytesRead, NULL);
		normals.SetData(verticesCount*sizeof(Vertex), norms, GL_STATIC_DRAW);
		delete [] norms;
	}

	if (hasTexCoords)
	{
		TexCoord *texs = new TexCoord[verticesCount];
		ReadFile(hFile, texs, verticesCount*sizeof(TexCoord), &bytesRead, NULL);
		texCoords.SetData(verticesCount*sizeof(TexCoord), texs, GL_STATIC_DRAW);
		delete [] texs;
	}

	CloseHandle(hFile);
	return true;
}