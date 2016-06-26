#include "mesh.h"
#include "datatypes.h"
#include "modelloader.h"

#define TEX_ID_NONE GLuint(-2)

Mesh::Mesh(GLRenderingContext *rc) : rc(rc)
{
	firstIndex = 0;
	indicesCount = -1;
	tangentsComputed = false;
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
	
	if (!tangents) tangents = VertexBuffer(rc, GL_ARRAY_BUFFER);
	if (!binormals) binormals = VertexBuffer(rc, GL_ARRAY_BUFFER);
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
	vao.Bind();
	if (material.diffuseMap) material.diffuseMap->Bind();
	if (material.specularIntensityMap) material.specularIntensityMap->Bind();
	if (material.normalMap) material.normalMap->Bind();

	indices->DrawElements(GL_TRIANGLES, GetIndicesCount(), GL_UNSIGNED_INT, firstIndex * sizeof(int));
}

void Mesh::DrawInstanced(int instanceCount)
{
	vao.Bind();
	if (material.diffuseMap) material.diffuseMap->Bind();
	if (material.specularIntensityMap) material.specularIntensityMap->Bind();
	if (material.normalMap) material.normalMap->Bind();

	indices->DrawElementsInstanced(GL_TRIANGLES, GetIndicesCount(),
		GL_UNSIGNED_INT, instanceCount, firstIndex * sizeof(int));
}

void Mesh::DrawFixed()
{
	if (material.diffuseMap) material.diffuseMap->Bind();

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

void Mesh::SetVertexFormat(int vfFlags)
{
	vao.Bind();

	if (vfFlags & VF_XYZ) {
		vao.EnableVertexAttrib(AttribLocation::Vertex);
		if (vertices)
			vertices->AttribPointer(AttribLocation::Vertex, 3, GL_FLOAT);
	}
	else {
		vao.DisableVertexAttrib(AttribLocation::Vertex);
	}

	if (vfFlags & VF_NORMAL) {
		vao.EnableVertexAttrib(AttribLocation::Normal);
		if (normals)
			normals->AttribPointer(AttribLocation::Normal, 3, GL_FLOAT);
	}
	else {
		vao.DisableVertexAttrib(AttribLocation::Normal);
	}

	if (vfFlags & VF_TEXCOORD) {
		vao.EnableVertexAttrib(AttribLocation::TexCoord);
		if (texCoords)
			texCoords->AttribPointer(AttribLocation::TexCoord, 2, GL_FLOAT);
	}
	else {
		vao.DisableVertexAttrib(AttribLocation::TexCoord);
	}
}

bool Mesh::LoadObj(const char *filename)
{
	ModelLoader loader(rc);
	bool result = loader.LoadObj(filename, *this);
	return result;
}

bool Mesh::LoadRaw(const char *filename)
{
	ModelLoader loader(rc);
	bool result = loader.LoadRaw(filename, *this);
	return result;
}