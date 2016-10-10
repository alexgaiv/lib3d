#include "modelloader.h"
#include "stringhelp.h"
#include "material.h"
#include <fstream>

#define RAW_FILE_SIGNATURE 0x00574152
#define RAW_HAS_NORMALS 1
#define RAW_HAS_TEXCOORDS 2

#pragma pack(push, 1)
struct RAWSTRING
{
	DWORD offset;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RAWHEADER
{
	DWORD signature;
	DWORD numVertices;
	DWORD numIndices;
	DWORD numMeshes;
	DWORD stringTableSize;
	DWORD flags;
	RAWSTRING materialLib;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RAWMESHDESC
{
	RAWSTRING materialName;
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

void ModelLoader::setupVao(vector<Mesh> &meshes, bool computeTangents)
{
	Mesh &m0 = meshes[0];

	int attribs = VA_XYZ;
	m0.vao.Bind();
	vertices->AttribPointer(AttribLocation::Vertex, 3, GL_FLOAT);
	if (normals) {
		attribs |= VA_NORMAL;
		normals->AttribPointer(AttribLocation::Normal, 3, GL_FLOAT);
	}
	if (texCoords) {
		attribs |= VA_TEXCOORD;
		texCoords->AttribPointer(AttribLocation::TexCoord, 2, GL_FLOAT);
	}
	m0.vao.EnableAttribs(attribs);

	if (!computeTangents) return;

	if (meshes.size() == 1)
	{
		m0.ComputeTangents();
		m0.vao.Bind();
		m0.vao.EnableVertexAttrib(AttribLocation::Tangent);
		m0.vao.EnableVertexAttrib(AttribLocation::Binormal);
		m0.tangents->AttribPointer(AttribLocation::Tangent, 3, GL_FLOAT);
		m0.binormals->AttribPointer(AttribLocation::Binormal, 3, GL_FLOAT);
	}
	else
	{
		VertexBuffer tangents(rc, GL_ARRAY_BUFFER);
		VertexBuffer binormals(rc, GL_ARRAY_BUFFER);
		VertexArrayObject vao_normalMap;

		vao_normalMap.Bind();
		vao_normalMap.EnableAttribs(attribs | VA_TANGENTS_BINORMALS);
		vertices->AttribPointer(AttribLocation::Vertex, 3, GL_FLOAT);
		tangents.AttribPointer(AttribLocation::Tangent, 3, GL_FLOAT);
		binormals.AttribPointer(AttribLocation::Binormal, 3, GL_FLOAT);
		if (normals)
			normals->AttribPointer(AttribLocation::Normal, 3, GL_FLOAT);
		if (texCoords)
			texCoords->AttribPointer(AttribLocation::TexCoord, 2, GL_FLOAT);

		int numVertices = vertices->GetSize() / sizeof(Vector3f);
		Vector3f *data = new Vector3f[numVertices];
		int dataSize = numVertices*sizeof(Vector3f);
		ZeroMemory(data, dataSize);
		tangents.SetData(dataSize, data, GL_STATIC_DRAW);
		binormals.SetData(dataSize, data, GL_STATIC_DRAW);
		delete[] data;

		for (int i = 0, s = meshes.size(); i < s; i++)
		{
			Mesh &m = meshes[i];
			m.tangents = tangents;
			m.binormals = binormals;
			if (m.material.normalMap) {
				m.vao = vao_normalMap;
				m.ComputeTangents();
			}
		}
	}
}

bool ModelLoader::loadObj(const char *filename, vector<Mesh> &meshes, bool separateMeshes)
{
	ifstream file(filename);
	if (!file) return false;

	Mesh mesh(rc);
	Dictionary<Material> *mtlLib = NULL;
	Material *curMaterial = NULL;
	string curMaterialName = "", lastMaterialName = "";
	bool computeTangents = false;

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
		line = strhlp::trimLeft(line);

		int i = line.find(' ');
		if (i == -1) continue;
		string prefix = line.substr(0, i);
		line = line.substr(i + 1);

		if (prefix == "mtllib") {
			mtlLib = rc->materials.GetLib(line.c_str());
			if (!mtlLib) {
				Dictionary<Material> materialLib;
				Dictionary<Texture2D> &textureLib = rc->textures.GetDefaultLib();
				MaterialLoader().LoadMtl(line.c_str(), materialLib, textureLib);
				mtlLib = &rc->materials.AddLib(line.c_str(), materialLib);
			}
		}
		else if (prefix == "usemtl")
		{
			if (curMaterialName != lastMaterialName)
			{
				if (curMaterial) {
					mesh.material = *curMaterial;
					lastMaterialName = curMaterialName;
				}
				mesh.SetFirstIndex(lastIndex);
				mesh.SetIndexCount(iverts.size() - lastIndex);
				mesh.boundingBox.vmin = vmin;
				mesh.boundingBox.vmax = vmax;
				mesh.boundingSphere.center = (vmax + vmin) / 2;
				mesh.boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);
				meshes.push_back(mesh);

				lastIndex = iverts.size();
				first_vert = true;
			}

			curMaterial = mtlLib->GetItem(line.c_str());
			if (curMaterial) {
				curMaterialName = line;
				if (curMaterial->normalMap) computeTangents = true;
			}
		}
		else if (prefix == "o" || prefix == "g")
		{
			if (!separateMeshes) continue;

			if (first_mesh)
				first_mesh = false;
			else {
				if (curMaterial) {
					mesh.material = *curMaterial;
					lastMaterialName = curMaterialName;
				}
				mesh.SetFirstIndex(lastIndex);
				mesh.SetIndexCount(iverts.size() - lastIndex);
				mesh.boundingBox.vmin = vmin;
				mesh.boundingBox.vmax = vmax;
				mesh.boundingSphere.center = (vmax + vmin) / 2;
				mesh.boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);
				meshes.push_back(mesh);

				lastIndex = iverts.size();
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
			int numVerts = verts.size();

			for (int k = 0; k < 3; k++) {
				read_num(line, c, i, n);
				iverts.push_back(n > 0 ? n - 1 : numVerts + n);

				if (c == '/')
				{
					bool skip = line[i] == '/';
					if (skip) i++;
					read_num(line, c, i, n);

					if (skip)
						inorms.push_back(n > 0 ? n - 1 : numVerts + n);
					else {
						itexs.push_back(n > 0 ? n - 1 : numVerts + n);
						if (c == '/') {
							read_num(line, c, i, n);
							inorms.push_back(n > 0 ? n - 1 : numVerts + n);
						}
					}
				}
			}
		}
	}

	if (curMaterial) mesh.material = *curMaterial;
	mesh.SetFirstIndex(lastIndex);
	mesh.SetIndexCount(iverts.size() - lastIndex);
	mesh.boundingBox.vmin = vmin;
	mesh.boundingBox.vmax = vmax;
	mesh.boundingSphere.center = (vmax + vmin) / 2;
	mesh.boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);
	meshes.push_back(mesh);

	file.close();

	if (meshes.size() == 1) meshes[0].SetIndexCount(-1);

	int numVertices = verts.size();
	if (numVertices == 0) return false;
	int numIndices = iverts.size();
	bool hasNormals = norms.size() != 0;
	bool hasTexCoords = texs.size() != 0;

	if (hasNormals || hasTexCoords)
	{
		vector<Vector3f> norms_new;
		vector<Vector2f> texs_new;

		if (hasNormals) {
			norms_new.resize(numVertices);
			memset(&norms_new[0], -1, numVertices * sizeof(Vector3f));
		}
		if (hasTexCoords) {
			texs_new.resize(numVertices);
			memset(&texs_new[0], -1, numVertices * sizeof(Vector2f));
		}

		for (int i = 0, k = iverts.size(); i < k; i++) {
			int iv = iverts[i];
			int in = hasNormals ? inorms[i] : 0;
			int it = hasTexCoords ? itexs[i] : 0;

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
				for (int j = numVertices, n = verts.size(); j < n; j++)
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
		numVertices = verts.size();

		if (hasNormals) {
			normals = VertexBuffer(rc, GL_ARRAY_BUFFER);
			normals->SetData(norms_new.size()*sizeof(Vector3f), norms_new.data(), GL_STATIC_DRAW);
		}
		if (hasTexCoords) {
			texCoords = VertexBuffer(rc, GL_ARRAY_BUFFER);
			texCoords->SetData(texs_new.size()*sizeof(Vector2f), texs_new.data(), GL_STATIC_DRAW);
		}
	}

	vertices = VertexBuffer(rc, GL_ARRAY_BUFFER);
	indices = VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	vertices->SetData(numVertices*sizeof(Vector3f), verts.data(), GL_STATIC_DRAW);
	indices->SetData(numIndices*sizeof(int), iverts.data(), GL_STATIC_DRAW);

	for (int i = 0, s = meshes.size(); i < s; i++)
	{
		Mesh &m = meshes[i];
		m.vertices = vertices;
		m.indices = indices;
		m.normals = normals;
		m.texCoords = texCoords;
	}

	setupVao(meshes, computeTangents);

	vertices = indices = texCoords = normals = NULL;
	return true;
}

bool ModelLoader::loadRaw(const char *filename, vector<Mesh> &meshes, bool separateMeshes)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	DWORD bytesRead = 0;
	RAWHEADER rawHeader = { };
	
	ReadFile(hFile, &rawHeader, sizeof(RAWHEADER), &bytesRead, NULL);

	if (rawHeader.signature != RAW_FILE_SIGNATURE) {
		CloseHandle(hFile);
		return false;
	}

	int numVertices = rawHeader.numVertices;
	int numIndices = rawHeader.numIndices;
	int numMeshes = rawHeader.numMeshes;
	bool hasNormals = (rawHeader.flags & RAW_HAS_NORMALS) != 0;
	bool hasTexCoords = (rawHeader.flags & RAW_HAS_TEXCOORDS) != 0;

	Dictionary<Material> *mtlLib = NULL;
	char *stringTable = NULL;
	RAWMESHDESC *meshDesc = new RAWMESHDESC[numMeshes];
	Vector3f *vertexData = new Vector3f[numVertices];
	UINT *indexData = new UINT[numIndices];

	if (rawHeader.stringTableSize != 0) {
		stringTable = new char[rawHeader.stringTableSize];
		ReadFile(hFile, stringTable, rawHeader.stringTableSize, &bytesRead, NULL);

		if (rawHeader.materialLib.offset != (DWORD)-1)
		{
			char *libName = stringTable + rawHeader.materialLib.offset;
			mtlLib = rc->materials.GetLib(libName);
			if (!mtlLib) {
				Dictionary<Material> materialLib;
				Dictionary<Texture2D> &textureLib = rc->textures.GetDefaultLib();
				MaterialLoader().LoadMtl(libName, materialLib, textureLib);
				mtlLib = &rc->materials.AddLib(libName, materialLib);
			}
		}
	}
	
	ReadFile(hFile, meshDesc, numMeshes*sizeof(RAWMESHDESC), &bytesRead, NULL);
	ReadFile(hFile, vertexData, numVertices*sizeof(Vector3f), &bytesRead, NULL);
	ReadFile(hFile, indexData, numIndices*sizeof(UINT), &bytesRead, NULL);

	vertices = VertexBuffer(rc, GL_ARRAY_BUFFER);
	indices = VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	vertices->SetData(numVertices*sizeof(Vector3f), vertexData, GL_STATIC_DRAW);
	indices->SetData(numIndices*sizeof(UINT), indexData, GL_STATIC_DRAW);

	if (hasNormals)
	{
		normals = VertexBuffer(rc, GL_ARRAY_BUFFER);
		Vector3f *norms = new Vector3f[numVertices];
		ReadFile(hFile, norms, numVertices*sizeof(Vector3f), &bytesRead, NULL);
		normals->SetData(numVertices*sizeof(Vector3f), norms, GL_STATIC_DRAW);
		delete[] norms;
	}

	if (hasTexCoords)
	{
		texCoords = VertexBuffer(rc, GL_ARRAY_BUFFER);
		Vector2f *texs = new Vector2f[numVertices];
		ReadFile(hFile, texs, numVertices*sizeof(Vector2f), &bytesRead, NULL);
		texCoords->SetData(numVertices*sizeof(Vector2f), texs, GL_STATIC_DRAW);
		delete[] texs;
	}

	CloseHandle(hFile);

	Mesh mesh(rc);
	mesh.vertices = vertices;
	mesh.indices = indices;
	mesh.normals = normals;
	mesh.texCoords = texCoords;

	bool computeTangents = false;
	for (int i = 0; i < numMeshes; i++)
	{
		if (mtlLib) {
			char *materialName = stringTable + meshDesc[i].materialName.offset;
			Material *material = mtlLib->GetItem(materialName);
			if (material) {
				mesh.material = *material;
				if (material->normalMap) computeTangents = true;
			}
			else mesh.material = Material();
		}

		mesh.SetFirstIndex(meshDesc[i].firstIndex);
		if (numMeshes != 1) {
			int next = i == numMeshes - 1 ? numIndices : meshDesc[i + 1].firstIndex;
			mesh.SetIndexCount(next - meshDesc[i].firstIndex);
		}

		Vector3f &vmin = meshDesc[i].vmin;
		Vector3f &vmax = meshDesc[i].vmax;

		mesh.boundingBox.vmin = vmin;
		mesh.boundingBox.vmax = vmax;
		mesh.boundingSphere.center = (vmax + vmin) / 2;
		mesh.boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);

		meshes.push_back(mesh);
	}

	setupVao(meshes, computeTangents);

	vertices = indices = normals = texCoords = NULL;
	delete [] stringTable;
	delete [] meshDesc;
	delete [] vertexData;
	delete [] indexData;
	return true;
}

bool ModelLoader::LoadObj(const char *filename, Mesh &mesh)
{
	vector<Mesh> tmp;
	bool result = loadObj(filename, tmp, false);
	if (result) mesh = tmp[0];
	return result;
}

bool ModelLoader::LoadObj(const char *filename, vector<Mesh> &meshes)
{
	return loadObj(filename, meshes, true);
}

bool ModelLoader::LoadRaw(const char *filename, Mesh &mesh)
{
	vector<Mesh> tmp;
	bool result = loadRaw(filename, tmp, false);
	if (result) mesh = tmp[0];
	return result;
}

bool ModelLoader::LoadRaw(const char *filename, vector<Mesh> &meshes)
{
	return loadRaw(filename, meshes, true);
}