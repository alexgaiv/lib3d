#include "modelloader.h"
#include <fstream>

#pragma pack(push, 1)
struct MeshDesc
{
	int firstIndex;
	Vector3f vmin;
	Vector3f vmax;
};
#pragma pack(pop)

void ModelLoader::read_num(const string &line, char &c, int &i, int &n)
{
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

bool ModelLoader::loadObj(const char *filename, vector<Mesh *> &meshes, bool separateMeshes)
{
	ifstream file(filename);
	if (!file) return false;

	MaterialLib *mtlLib = NULL;
	Material *curMaterial = NULL;

	VertexBuffer *vertices = NULL;
	VertexBuffer *indices = NULL;
	VertexBuffer *texCoords = NULL;
	VertexBuffer *normals = NULL;

	Vector3f v;
	Vector2f tc;

	vector<Vector3f> verts, norms;
	vector<Vector2f> texs;
	vector<int> iverts, inorms, itexs;

	Vector3f vmax, vmin;
	bool first_vert = true;
	bool first_mesh = true;
	int lastIndex = 0;

	string line;
	while (getline(file, line))
	{
		int k = 0;
		while (isspace(line[k])) k++;
		if (k != 0) line.erase(0, k);

		int i = line.find(' ');
		if (i == -1) continue;
		string prefix = line.substr(0, i);
		line = line.substr(i + 1);

		if (prefix == "mtllib") {
			map<string, MaterialLib>::iterator i = rc->materials.find(line);
			if (i != rc->materials.end()) {
				mtlLib = &i->second;
			}
			else {
				mtlLib = new MaterialLib(rc);
				mtlLib->LoadMtl(line.c_str());
				rc->materials.insert(make_pair(line, *mtlLib));
			}
		}
		else if (prefix == "usemtl") {
			curMaterial = mtlLib->GetMaterial(line.c_str());
		}
		else if (prefix == "o" || prefix == "g")
		{
			if (!separateMeshes) continue;

			if (first_mesh)
				first_mesh = false;
			else {
				Mesh *m = new Mesh(rc);
				if (curMaterial) m->material = Material(*curMaterial);
				m->SetFirstIndex(lastIndex);
				m->SetIndicesCount(iverts.size() - lastIndex);
				lastIndex = iverts.size();
				meshes.push_back(m);

				m->boundingBox.vmin = vmin;
				m->boundingBox.vmax = vmax;
				m->boundingSphere.center = (vmax + vmin) / 2;
				m->boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);

				first_vert = true;
			}
		}
		else if (prefix == "v") {
			sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			verts.push_back(v);

			if (first_vert) {
				vmax = vmin = v;
				first_vert = false;
			}
			else {
				if (v.x > vmax.x) vmax.x = v.x;
				if (v.y > vmax.y) vmax.y = v.y;
				if (v.z > vmax.z) vmax.z = v.z;

				if (v.x < vmin.x) vmin.x = v.x;
				if (v.y < vmin.y) vmin.y = v.y;
				if (v.z < vmin.z) vmin.z = v.z;
			}
		}
		else if (prefix == "vn") {
			sscanf_s(line.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			norms.push_back(v);	 
		}
		else if (prefix == "vt") {
			sscanf_s(line.c_str(), "%f %f", &tc.x, &tc.y);
			texs.push_back(tc);
		}
		else if (prefix == "f")
		{
			char c = 0;
			int i = 0;
			int n = 0;
			int vertsCount = verts.size();

			for (int k = 0; k < 3; k++) {
				read_num(line, c, i, n);
				iverts.push_back(n > 0 ? n - 1 : vertsCount + n);

				if (c == '/')
				{
					bool skip = line[i] == '/';
					if (skip) i++;
					read_num(line, c, i, n);

					if (skip)
						inorms.push_back(n > 0 ? n - 1 : vertsCount + n);
					else {
						itexs.push_back(n > 0 ? n - 1 : vertsCount + n);
						if (c == '/') {
							read_num(line, c, i, n);
							inorms.push_back(n > 0 ? n - 1 : vertsCount + n);
						}
					}
				}
			}
		}
	}

	file.close();

	Mesh *m = new Mesh(rc);
	if (curMaterial) m->material = Material(*curMaterial);
	m->SetFirstIndex(lastIndex);
	m->SetIndicesCount(iverts.size() - lastIndex);
	meshes.push_back(m);

	m->boundingBox.vmin = vmin;
	m->boundingBox.vmax = vmax;
	m->boundingSphere.center = (vmax + vmin) / 2;
	m->boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);

	int verticesCount = verts.size();
	if (verticesCount == 0) return false;
	int indicesCount = iverts.size();
	bool hasNormals = norms.size() != 0;
	bool hasTexCoords = texs.size() != 0;

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

		if (hasNormals) {
			normals = new VertexBuffer(rc, GL_ARRAY_BUFFER);
			normals->SetData(norms_new.size()*sizeof(Vector3f), norms_new.data(), GL_STATIC_DRAW);
		}
		if (hasTexCoords) {
			texCoords = new VertexBuffer(rc, GL_ARRAY_BUFFER);
			texCoords->SetData(texs_new.size()*sizeof(Vector2f), texs_new.data(), GL_STATIC_DRAW);
		}
	}

	vertices = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	indices = new VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	vertices->SetData(verticesCount*sizeof(Vector3f), verts.data(), GL_STATIC_DRAW);
	indices->SetData(indicesCount*sizeof(int), iverts.data(), GL_STATIC_DRAW);

	for (int i = 0, s = meshes.size(); i < s; i++)
	{
		Mesh *mesh = meshes[i];
		int format = VF_XYZ;

		mesh->vertices = *vertices;
		mesh->indices = *indices;
		if (normals) {
			mesh->normals = *normals;
			format |= VF_NORMAL;
		}
		if (texCoords) {
			mesh->texCoords = *texCoords;
			format |= VF_TEXCOORD;
		}

		mesh->SetVertexFormat(format);
	}

	if (meshes.size() == 1) meshes[0]->SetIndicesCount(-1);

	delete vertices;
	delete indices;
	delete normals;
	delete texCoords;
	return true;
}

bool ModelLoader::loadRaw(const char *filename, vector<Mesh *> &meshes, bool separateMeshes)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	DWORD bytesRead = 0;

	BYTE signature[4] = { };
	ReadFile(hFile, signature, 3, &bytesRead, NULL);
	if (*(int *)signature != 0x00574152) return false;

	int verticesCount = 0;
	int indicesCount = 0;
	bool hasNormals = false;
	bool hasTexCoords = false;
	int numMeshes = 0;

	ReadFile(hFile, &verticesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &indicesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &hasNormals, 1, &bytesRead, NULL);
	ReadFile(hFile, &hasTexCoords, 1, &bytesRead, NULL);
	ReadFile(hFile, &numMeshes, sizeof(int), &bytesRead, NULL);

	Mesh mm(rc);
	mm.vertices = VertexBuffer(rc, GL_ARRAY_BUFFER);
	mm.indices = VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	if (hasNormals) mm.normals = VertexBuffer(rc, GL_ARRAY_BUFFER);
	if (hasTexCoords) mm.texCoords = VertexBuffer(rc, GL_ARRAY_BUFFER);

	MeshDesc *meshDesc = new MeshDesc[numMeshes];
	ReadFile(hFile, meshDesc, numMeshes*sizeof(MeshDesc), &bytesRead, NULL);

	for (int i = 0; i < numMeshes; i++)
	{
		Mesh *m = new Mesh(mm);
		meshes.push_back(m);

		m->SetFirstIndex(meshDesc[i].firstIndex);
		if (numMeshes != 1) {
			int next = i == numMeshes - 1 ? indicesCount : meshDesc[i + 1].firstIndex;
			m->SetIndicesCount(next - meshDesc[i].firstIndex);
		}

		Vector3f &vmin = meshDesc[i].vmin;
		Vector3f &vmax = meshDesc[i].vmax;

		m->boundingBox.vmin = vmin;
		m->boundingBox.vmax = vmax;
		m->boundingSphere.center = (vmax + vmin) / 2;
		m->boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);

		int format = VF_XYZ;
		if (hasNormals) format |= VF_NORMAL;
		if (hasTexCoords) format |= VF_TEXCOORD;
		m->SetVertexFormat(format);
	}
	delete [] meshDesc;

	Vector3f *verts = new Vector3f[verticesCount];
	UINT *indx = new UINT[indicesCount];
	ReadFile(hFile, verts, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
	ReadFile(hFile, indx, indicesCount*sizeof(UINT), &bytesRead, NULL);
	
	mm.vertices->SetData(verticesCount*sizeof(Vector3f), verts, GL_STATIC_DRAW);
	mm.indices->SetData(indicesCount*sizeof(UINT), indx, GL_STATIC_DRAW);

	delete [] verts;
	delete [] indx;

	if (hasNormals)
	{
		Vector3f *norms = new Vector3f[verticesCount];
		ReadFile(hFile, norms, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
		mm.normals->SetData(verticesCount*sizeof(Vector3f), norms, GL_STATIC_DRAW);
		delete [] norms;
	}

	if (hasTexCoords)
	{
		Vector2f *texs = new Vector2f[verticesCount];
		ReadFile(hFile, texs, verticesCount*sizeof(Vector2f), &bytesRead, NULL);
		mm.texCoords->SetData(verticesCount*sizeof(Vector2f), texs, GL_STATIC_DRAW);
		delete [] texs;
	}

	CloseHandle(hFile);
	return true;
}

bool ModelLoader::LoadObj(const char *filename, Mesh &mesh)
{
	vector<Mesh *> tmp;
	bool result = loadObj(filename, tmp, false);
	if (result) mesh = *tmp[0];
	return result;
}

bool ModelLoader::LoadObj(const char *filename, vector<Mesh *> &meshes)
{
	return loadObj(filename, meshes, true);
}

bool ModelLoader::LoadRaw(const char *filename, Mesh &mesh)
{
	vector<Mesh *> tmp;
	bool result = loadRaw(filename, tmp, false);
	if (result) mesh = *tmp[0];
	return result;
}

bool ModelLoader::LoadRaw(const char *filename, vector<Mesh *> &meshes)
{
	return loadRaw(filename, meshes, true);
}