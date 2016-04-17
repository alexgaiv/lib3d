#include "model.h"

Model::Model(GLRenderingContext *rc)
	: rc(rc), scale(Vector3f(1.0f)), program(NULL) { }

Model::Model(const Model &m) {
	clone(m);
}

Model::~Model() {
	cleanup();
}

Model &Model::operator=(const Model &m) {
	cleanup();
	clone(m);
	return *this;
}

void Model::clone(const Model &m)
{
	rc = m.rc;
	transform = m.transform;
	location = m.location;
	rotation = m.rotation;
	scale = m.scale;
	program = m.program ? new ProgramObject(*m.program) : NULL;

	int s = m.meshes.size();
	meshes.reserve(s);
	for (int i = 0; i < s; i++)
		meshes.push_back(new Mesh(*m.meshes[i]));
}

void Model::cleanup() {
	delete program;
	for (int i = 0, s = meshes.size(); i < s; i++)
		delete meshes[i];
}

void Model::read_num(const string &line, char &c, int &i, int &n) {
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

bool Model::LoadObj(const char *filename)
{
	ifstream file(filename);
	if (!file) return false;

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
		if (line.size() < 2) continue;

		string prefix = line.substr(0, 2);
		line[0] = line[1] = ' ';

		int s = 2;
		while (isspace(line[s++]));
		line = line.substr(s - 1);

		if (prefix == "o ") {
			if (first_mesh)
				first_mesh = false;
			else {
				Mesh *m = new Mesh(rc);
				m->SetFirstIndex(lastIndex);
				m->SetIndicesCount(iverts.size() - lastIndex);
				lastIndex = iverts.size();
				meshes.push_back(m);

				m->boundingBox.front = Plane(0, 0, 1, -vmax.z);
				m->boundingBox.back = Plane(0, 0, -1, vmin.z);
				m->boundingBox.top = Plane(0, 1, 0, -vmax.y);
				m->boundingBox.bottom = Plane(0, -1, 0, vmin.y);
				m->boundingBox.left = Plane(-1, 0, 0, vmin.x);
				m->boundingBox.right = Plane(1, 0, 0, -vmax.x);

				m->boundingSphere.center = (vmax + vmin) / 2;
				m->boundingSphere.radius = max(max(vmax.x - vmin.x, vmax.y - vmin.y), vmax.z - vmin.z);

				first_vert = true;
			}
			
		}
		else if (prefix == "v ") {
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

	Mesh *m = new Mesh(rc);
	m->SetFirstIndex(lastIndex);
	m->SetIndicesCount(iverts.size() - lastIndex);
	meshes.push_back(m);

	m->boundingBox.front = Plane(0, 0, 1, -vmax.z);
	m->boundingBox.back = Plane(0, 0, -1, vmin.z);
	m->boundingBox.top = Plane(0, 1, 0, -vmax.y);
	m->boundingBox.bottom = Plane(0, -1, 0, vmin.y);
	m->boundingBox.left = Plane(-1, 0, 0, vmin.x);
	m->boundingBox.right = Plane(1, 0, 0, -vmax.x);

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
		mesh->vertices = new VertexBuffer(*vertices);
		mesh->indices = new VertexBuffer(*indices);
		if (normals)
			mesh->normals = new VertexBuffer(*normals);
		if (texCoords)
			mesh->texCoords = new VertexBuffer(*texCoords);
	}

	if (meshes.size() == 1) meshes[0]->SetIndicesCount(-1);

	delete vertices;
	delete indices;
	delete normals;
	delete texCoords;
	return true;
}