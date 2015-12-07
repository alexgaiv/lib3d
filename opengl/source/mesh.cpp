#include "mesh.h"
#include "datatypes.h"

#define TEX_ID_NONE GLuint(-2)

Mesh::Mesh()
	: texture(GL_TEXTURE_2D, TEX_OWN_ID, TEX_ID_NONE), programId(0)
{
	verticesCount = indicesCount = 0;
	indices.SetTarget(GL_ELEMENT_ARRAY_BUFFER);
}

void Mesh::BindTexture(const BaseTexture &texture) {
	this->texture = texture;
}

void Mesh::BindShader(const ProgramObject &program) {
	this->programId = program.Handle();
}

void Mesh::Draw(int first, int count)
{
	if (texture.GetId() != TEX_ID_NONE)
		texture.Bind();

	if (programId && programId != Global::curProgram)
		glUseProgram(programId);

	glEnableVertexAttribArray(0);
	vertices.Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (hasNormals) {
		glEnableVertexAttribArray(1);
		normals.Bind();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (hasTexCoords) {
		glEnableVertexAttribArray(2);
		texCoords.Bind();
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	indices.Bind();
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
	vertices.Bind();
	glVertexPointer(3, GL_FLOAT, 0, 0);

	if (hasNormals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		normals.Bind();
		glNormalPointer(GL_FLOAT, 0, 0);
	}

	if (hasTexCoords) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		texCoords.Bind();
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	indices.Bind();
	glDrawElements(GL_TRIANGLES, count == -1 ? indicesCount : count, GL_UNSIGNED_INT, (void *)first);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

bool Mesh::LoadObj(const char *filename)
{
	ifstream file(filename);
	if (!file) return false;

	Vector3f v;
	Vector2f tc;

	vector<Vector3f> verts, norms;
	vector<Vector2f> texs;
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
			sscanf_s(line.c_str(), "%f %f", &tc.x, &tc.y);
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
		vector<Vector3f> norms_new;
		vector<Vector2f> texs_new;

		if (hasNormals) {
			norms_new.resize(verticesCount);
			memset(&norms_new[0], -1, verticesCount * sizeof(Vector3f));
		}
		if (hasTexCoords) {
			texs_new.resize(verticesCount);
			memset(&texs_new[0], -1, verticesCount * sizeof(Vector2f));
		}

		for (int i = 0, k = iverts.size(); i < k; i++) {
			int iv = iverts[i];
			int it = hasTexCoords ? itexs[i] : 0;
			int in = hasNormals ? inorms[i] : 0;

			if ((!hasNormals || *(int *)&norms_new[iv].x == -1) &&
				(!hasTexCoords || *(int *)&texs_new[iv].x == -1))
			{
				if (hasNormals) norms_new[iv] = norms[in];
				if (hasTexCoords) texs_new[iv] = texs[it];
			}
			else if (hasNormals && norms_new[iv] != norms[in] ||
				     hasTexCoords && texs_new[iv] != texs[it])
			{
 				int same = -1;
				for (int j = verticesCount, n = verts.size(); j < n; j++)
				{
					if (verts[j] == verts[iv] &&
						(!hasNormals || norms_new[j] == norms[in]) &&
						(!hasTexCoords || texs_new[j] == texs[it]))
					{
						same = j;
						break;
					}
				}

				if (same != -1) iverts[i] = same;
				else {
					iverts[i] = verts.size();
					verts.push_back(verts[iv]);
					if (hasNormals) norms_new.push_back(norms[in]);
					if (hasTexCoords) texs_new.push_back(texs[it]);
				}
			}
		}

		verticesCount = verts.size();

		if (hasNormals)
			normals.SetData(norms_new.size()*sizeof(Vector3f), norms_new.data(), GL_STATIC_DRAW);
		if (hasTexCoords)
			texCoords.SetData(texs_new.size()*sizeof(Vector2f), texs_new.data(), GL_STATIC_DRAW);
	}
	
	vertices.SetData(verticesCount*sizeof(Vector3f), verts.data(), GL_STATIC_DRAW);
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

	Vector3f *verts = new Vector3f[verticesCount];
	UINT *indx = new UINT[indicesCount];

	ReadFile(hFile, verts, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
	ReadFile(hFile, indx, indicesCount*sizeof(UINT), &bytesRead, NULL);

	vertices.SetData(verticesCount*sizeof(Vector3f), verts, GL_STATIC_DRAW);
	indices.SetData(indicesCount*sizeof(UINT), indx, GL_STATIC_DRAW);

	delete [] verts;
	delete [] indx;

	if (hasNormals)
	{
		Vector3f *norms = new Vector3f[verticesCount];
		ReadFile(hFile, norms, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
		normals.SetData(verticesCount*sizeof(Vector3f), norms, GL_STATIC_DRAW);
		delete [] norms;
	}

	if (hasTexCoords)
	{
		Vector2f *texs = new Vector2f[verticesCount];
		ReadFile(hFile, texs, verticesCount*sizeof(Vector2f), &bytesRead, NULL);
		texCoords.SetData(verticesCount*sizeof(Vector2f), texs, GL_STATIC_DRAW);
		delete [] texs;
	}

	CloseHandle(hFile);
	return true;
}