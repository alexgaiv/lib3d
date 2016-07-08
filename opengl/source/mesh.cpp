#include "mesh.h"
#include "datatypes.h"
#include "modelloader.h"

#define TEX_ID_NONE GLuint(-2)

Mesh::Mesh(GLRenderingContext *rc) : rc(rc)
{
	firstIndex = 0;
	numIndices = -1;
	//vertexFormat = 0;
}

void Mesh::RecalcTangents()
{
	if (!normals || !texCoords) return;

	int numVertices = GetVertexCount();
	int numIndices = GetIndexCount();

	if (!tangents || !binormals)
	{
		tangents = VertexBuffer(rc, GL_ARRAY_BUFFER);
		binormals = VertexBuffer(rc, GL_ARRAY_BUFFER);

		Vector3f *data = new Vector3f[numVertices];
		int dataSize = numVertices*sizeof(Vector3f);
		ZeroMemory(data, dataSize);
		tangents->SetData(dataSize, data, GL_STATIC_DRAW);
		binormals->SetData(dataSize, data, GL_STATIC_DRAW);
		delete[] data;
	}

	Vector3f *verts = (Vector3f *)vertices->Map(GL_READ_ONLY);
	Vector2f *texs = (Vector2f *)texCoords->Map(GL_READ_ONLY);
	int *inds = (int *)indices->Map(GL_READ_ONLY);
	Vector3f *ts = (Vector3f *)tangents->Map(GL_READ_WRITE);
	Vector3f *bs = (Vector3f *)binormals->Map(GL_READ_WRITE);
	
	for (int i = firstIndex, n = firstIndex + numIndices; i < n; i += 3)
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

	vertices->Unmap();
	texCoords->Unmap();
	indices->Unmap();
	tangents->Unmap();
	binormals->Unmap();
}

void Mesh::Draw()
{
	if (!indices) return;

	vao.Bind();
	ProgramObject *p = rc->GetCurProgram();

	if (material.diffuseMap) {
		p->Uniform("Material.useDiffuseMap", 1);
		material.diffuseMap->Bind();
	}
	if (material.normalMap) {
		p->Uniform("Material.useNormalMap", 1);
		material.normalMap->Bind();
	}
	if (material.opacityMask) {
		p->Uniform("Material.useOpacityMask", 1);
		material.opacityMask->Bind();
	}

	p->Uniform("Material.ambient", 1, material.ambient.data);
	p->Uniform("Material.diffuse", 1, material.diffuse.data);
	p->Uniform("Material.mode", (int)material.mode);

	if (material.mode == MM_BLINN_PHONG)
	{
		p->Uniform("Material.specular", 1, material.specular.data);
		p->Uniform("Material.shininess", material.specularIntensity);
		if (material.specularMap) {
			p->Uniform("Material.useSpecularMap", 1);
			material.specularMap->Bind();
		}
	}

	indices->DrawElements(GL_TRIANGLES, GetIndexCount(), GL_UNSIGNED_INT, firstIndex * sizeof(int));

	p->Uniform("Material.useDiffuseMap", 0);
	p->Uniform("Material.useSpecularMap", 0);
	p->Uniform("Material.useNormalMap", 0);
	p->Uniform("Material.useOpacityMask", 0);
}

void Mesh::DrawInstanced(int instanceCount)
{
	if (!indices) return;

	vao.Bind();
	if (material.diffuseMap) material.diffuseMap->Bind();
	if (material.specularIntensityMap) material.specularIntensityMap->Bind();
	if (material.normalMap) material.normalMap->Bind();

	indices->DrawElementsInstanced(GL_TRIANGLES, GetIndexCount(),
		GL_UNSIGNED_INT, instanceCount, firstIndex * sizeof(int));
}

void Mesh::DrawFixed()
{
	if (!indices) return;
	if (material.diffuseMap) material.diffuseMap->Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	vertices->VertexPointer(3, GL_FLOAT, 0);

	if (normals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		normals->NormalPointer(GL_FLOAT, 0);
	}

	if (texCoords) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		texCoords->TexCoordPointer(2, GL_FLOAT, 0);
	}

	indices->DrawElements(GL_TRIANGLES, GetIndexCount(), GL_UNSIGNED_INT, firstIndex);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::SetVertexFormat(int vfFlags)
{
	//if (vao.GetRefCount() > 1)
		//vao = VertexArrayObject();

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

	if (vfFlags & VF_TANGENTS_BINORMALS) {
		vao.EnableVertexAttrib(AttribLocation::Tangent);
		vao.EnableVertexAttrib(AttribLocation::Binormal);
		if (tangents)
			tangents->AttribPointer(AttribLocation::Tangent, 3, GL_FLOAT);
		if (binormals)
			binormals->AttribPointer(AttribLocation::Binormal, 3, GL_FLOAT);
	}
	else {
		vao.DisableVertexAttrib(AttribLocation::Tangent);
		vao.DisableVertexAttrib(AttribLocation::Binormal);
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