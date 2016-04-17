#include "mesh.h"
#include "datatypes.h"

#define TEX_ID_NONE GLuint(-2)

Mesh::Mesh(GLRenderingContext *rc) : rc(rc)
{
	firstIndex = 0;
	indicesCount = -1;

	vertices = indices = normals = texCoords = NULL;
	tangents = binormals = NULL;
	texture = NULL;
	normalMap = specularMap = NULL;
	tangentsComputed = false;
}

Mesh::Mesh(const Mesh &m) {
	clone(m);
}

Mesh::~Mesh() {
	cleanup();
}

Mesh &Mesh::operator=(const Mesh &m) {
	cleanup();
	clone(m);
	return *this;
}

void Mesh::clone(const Mesh &m)
{
	rc = m.rc;

	firstIndex = m.firstIndex;
	indicesCount = m.indicesCount;

	vertices = m.vertices ? new VertexBuffer(*m.vertices) : NULL;
	indices = m.indices ? new VertexBuffer(*m.indices) : NULL;
	normals = m.normals ? new VertexBuffer(*m.normals) : NULL;
	texCoords = m.texCoords  ? new VertexBuffer(*m.texCoords) : NULL;
	tangents = m.tangents ? new VertexBuffer(*m.tangents) : NULL;
	binormals = m.binormals ? new VertexBuffer(*m.binormals) : NULL;

	texture = m.texture ? new BaseTexture(*m.texture) : NULL;
	normalMap = m.normalMap ? new Texture2D(*m.normalMap) : NULL;
	specularMap = m.specularMap ? new Texture2D(*m.specularMap) : NULL;

	tangentsComputed = m.tangentsComputed;
	boundingBox = m.boundingBox;
	boundingSphere = m.boundingSphere;
}

void Mesh::cleanup()
{
	delete vertices;
	delete indices;
	delete normals;
	delete texCoords;
	delete tangents;
	delete binormals;
	delete texture;
	delete normalMap;
	delete specularMap;
}

void Mesh::BindTexture(const BaseTexture &texture) {
	delete this->texture;
	this->texture = new BaseTexture(texture);
}

void Mesh::BindNormalMap(const Texture2D &normalMap) {
	delete this->normalMap;
	this->normalMap = new Texture2D(normalMap);
	if (!tangentsComputed) {
		RecalcTangents();
		tangentsComputed = true;
	}
}

void Mesh::BindSpecularMap(const Texture2D &specularMap) {
	delete this->specularMap;
	this->specularMap = new Texture2D(specularMap);
}

void Mesh::RecalcTangents()
{
	if (!HasNormals() || !HasTexCoords()) return;

	Vector3f *verts = (Vector3f *)vertices->Map(GL_READ_ONLY);
	Vector2f *texs = (Vector2f *)texCoords->Map(GL_READ_ONLY);
	int *inds = (int *)indices->Map(GL_READ_ONLY);
	
	int verticesCount = GetVerticesCount();
	int indicesCount = GetIndicesCount();
	Vector3f *ts = new Vector3f[verticesCount];
	Vector3f *bs = new Vector3f[verticesCount];

	for (int i = 0; i < indicesCount; i += 3)
	{
		int i1 = inds[i], i2 = inds[i+1], i3 = inds[i+2];

		Vector3f &v1 = verts[i1];
		Vector3f &v2 = verts[i2];
		Vector3f &v3 = verts[i3];

		Vector2f &t1 = texs[i1];
		Vector2f &t2 = texs[i2];
		Vector2f &t3 = texs[i3];

		Vector3f edge1 = v2 - v1;
		Vector3f edge2 = v3 - v1;
		Vector2f uv1 = t2 - t1;
		Vector2f uv2 = t3 - t1;

		float f = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);
		Vector3f tangent = (uv2.y * edge1 - uv1.y * edge2) * f;
		Vector3f binormal = (uv1.x * edge2 - uv2.x * edge1) * f;
		tangent.Normalize();
		binormal.Normalize();

		ts[i1] = ts[i2] = ts[i3] = tangent;
		bs[i1] = bs[i2] = bs[i3] = binormal;
	}
	
	if (!tangents) tangents = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	if (!binormals) binormals = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	tangents->SetData(verticesCount*sizeof(Vector3f), ts, GL_STATIC_DRAW);
	binormals->SetData(verticesCount*sizeof(Vector3f), bs, GL_STATIC_DRAW);

	delete [] ts;
	delete [] bs;

	vertices->Unmap();
	texCoords->Unmap();
	indices->Unmap();
}

void Mesh::Draw()
{
	if (texture) texture->Bind();
	if (specularMap) specularMap->Bind();
	if (normalMap) {
		normalMap->Bind();
		glEnableVertexAttribArray(AttribsLocations.Tangent);
		glEnableVertexAttribArray(AttribsLocations.Binormal);
		tangents->AttribPointer(AttribsLocations.Tangent, 3, GL_FLOAT);
		binormals->AttribPointer(AttribsLocations.Binormal, 3, GL_FLOAT);
	}

	glEnableVertexAttribArray(AttribsLocations.Vertex);
	vertices->AttribPointer(AttribsLocations.Vertex, 3, GL_FLOAT);

	if (HasNormals()) {
		glEnableVertexAttribArray(AttribsLocations.Normal);
		normals->AttribPointer(AttribsLocations.Normal, 3, GL_FLOAT);
	}

	if (HasTexCoords()) {
		glEnableVertexAttribArray(AttribsLocations.TexCoord);
		texCoords->AttribPointer(AttribsLocations.TexCoord, 2, GL_FLOAT);
	}

	indices->DrawElements(GL_TRIANGLES, GetIndicesCount(), GL_UNSIGNED_INT, firstIndex * sizeof(int));

	glDisableVertexAttribArray(AttribsLocations.Vertex);
	glDisableVertexAttribArray(AttribsLocations.Normal);
	glDisableVertexAttribArray(AttribsLocations.TexCoord);
	if (normalMap) {
		glDisableVertexAttribArray(AttribsLocations.Tangent);
		glDisableVertexAttribArray(AttribsLocations.Binormal);
	}
}

void Mesh::DrawInstanced(int instanceCount)
{
	if (texture) texture->Bind();
	if (specularMap) specularMap->Bind();
	if (normalMap) {
		normalMap->Bind();
		glEnableVertexAttribArray(AttribsLocations.Tangent);
		glEnableVertexAttribArray(AttribsLocations.Binormal);
		tangents->AttribPointer(AttribsLocations.Tangent, 3, GL_FLOAT);
		binormals->AttribPointer(AttribsLocations.Binormal, 3, GL_FLOAT);
	}

	glEnableVertexAttribArray(AttribsLocations.Vertex);
	vertices->AttribPointer(AttribsLocations.Vertex, 3, GL_FLOAT);

	if (HasNormals()) {
		glEnableVertexAttribArray(AttribsLocations.Normal);
		normals->AttribPointer(AttribsLocations.Normal, 3, GL_FLOAT);
	}

	if (HasTexCoords()) {
		glEnableVertexAttribArray(AttribsLocations.TexCoord);
		texCoords->AttribPointer(AttribsLocations.TexCoord, 2, GL_FLOAT);
	}

	indices->DrawElementsInstanced(GL_TRIANGLES, GetIndicesCount(),
		GL_UNSIGNED_INT, instanceCount, firstIndex * sizeof(int));

	glDisableVertexAttribArray(AttribsLocations.Vertex);
	glDisableVertexAttribArray(AttribsLocations.Normal);
	glDisableVertexAttribArray(AttribsLocations.TexCoord);
	if (normalMap) {
		glDisableVertexAttribArray(AttribsLocations.Tangent);
		glDisableVertexAttribArray(AttribsLocations.Binormal);
	}
}

void Mesh::DrawFixed()
{
	if (texture) texture->Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	vertices->VertexPointer(3, GL_FLOAT, 0);

	if (HasNormals()) {
		glEnableClientState(GL_NORMAL_ARRAY);
		normals->NormalPointer(GL_FLOAT, 0);
	}

	if (HasTexCoords()) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		texCoords->TexCoordPointer(2, GL_FLOAT, 0);
	}

	indices->DrawElements(GL_TRIANGLES, GetIndicesCount(), GL_UNSIGNED_INT, firstIndex);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::read_num(const string &line, char &c, int &i, int &n) {
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

bool Mesh::LoadObj(const char *filename)
{
	ifstream file(filename);
	if (!file) return false;

	Vector3f v;
	Vector2f tc;

	vector<Vector3f> verts, norms;
	vector<Vector2f> texs;
	vector<int> iverts, inorms, itexs;

	Vector3f vmax, vmin;
	bool first = true;

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

			if (first) {
				vmax = vmin = v;
				first = false;
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
		else if (prefix == "f ")
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

	int verticesCount = verts.size();
	if (verticesCount == 0) return false;
	this->indicesCount = iverts.size();
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
			if (!normals) normals = new VertexBuffer(rc, GL_ARRAY_BUFFER);
			normals->SetData(norms_new.size()*sizeof(Vector3f), norms_new.data(), GL_STATIC_DRAW);
		}
		if (hasTexCoords) {
			if (!texCoords) texCoords = new VertexBuffer(rc, GL_ARRAY_BUFFER);
			texCoords->SetData(texs_new.size()*sizeof(Vector2f), texs_new.data(), GL_STATIC_DRAW);
		}
	}
	
	if (!vertices) vertices = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	if (!indices) indices = new VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	vertices->SetData(verticesCount*sizeof(Vector3f), verts.data(), GL_STATIC_DRAW);
	indices->SetData(indicesCount*sizeof(int), iverts.data(), GL_STATIC_DRAW);

	boundingBox.front = Plane(0, 0, 1, -vmax.z);
	boundingBox.back = Plane(0, 0, -1, vmin.z);
	boundingBox.top = Plane(0, 1, 0, -vmax.y);
	boundingBox.bottom = Plane(0, -1, 0, vmin.y);
	boundingBox.left = Plane(-1, 0, 0, vmin.x);
	boundingBox.right = Plane(1, 0, 0, -vmax.x);

	boundingSphere.center = (vmax + vmin) / 2;
	boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);
	
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

	int verticesCount = 0;
	bool hasNormals = false;
	bool hasTexCoords = false;

	ReadFile(hFile, &verticesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &this->indicesCount, sizeof(int), &bytesRead, NULL);
	ReadFile(hFile, &hasNormals, 1, &bytesRead, NULL);
	ReadFile(hFile, &hasTexCoords, 1, &bytesRead, NULL);

	Vector3f *verts = new Vector3f[verticesCount];
	UINT *indx = new UINT[indicesCount];

	ReadFile(hFile, verts, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
	ReadFile(hFile, indx, indicesCount*sizeof(UINT), &bytesRead, NULL);

	if (!vertices) vertices = new VertexBuffer(rc, GL_ARRAY_BUFFER);
	if (!indices) indices = new VertexBuffer(rc, GL_ELEMENT_ARRAY_BUFFER);
	vertices->SetData(verticesCount*sizeof(Vector3f), verts, GL_STATIC_DRAW);
	indices->SetData(indicesCount*sizeof(UINT), indx, GL_STATIC_DRAW);

	delete [] verts;
	delete [] indx;

	if (hasNormals)
	{
		Vector3f *norms = new Vector3f[verticesCount];
		ReadFile(hFile, norms, verticesCount*sizeof(Vector3f), &bytesRead, NULL);
		if (!normals) normals = new VertexBuffer(rc, GL_ARRAY_BUFFER);
		normals->SetData(verticesCount*sizeof(Vector3f), norms, GL_STATIC_DRAW);
		delete [] norms;
	}

	if (hasTexCoords)
	{
		Vector2f *texs = new Vector2f[verticesCount];
		ReadFile(hFile, texs, verticesCount*sizeof(Vector2f), &bytesRead, NULL);
		if (!texCoords) texCoords = new VertexBuffer(rc, GL_ARRAY_BUFFER);
		texCoords->SetData(verticesCount*sizeof(Vector2f), texs, GL_STATIC_DRAW);
		delete [] texs;
	}

	CloseHandle(hFile);
	return true;
}