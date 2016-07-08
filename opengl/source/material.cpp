#include "material.h"
#include "glcontext.h"
#include <utility>

Texture2D MaterialLoader::getTexture(const string &name, Dictionary<Texture2D> &textures)
{
	Texture2D *tex = textures.GetItem(name.c_str());
	if (tex) return *tex;

	Texture2D t(name.c_str());
	t.SetFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	t.BuildMipmaps();
	return textures.AddItem(name.c_str(), t);
}

bool MaterialLoader::LoadMtl(const char *filename, Dictionary<Material> &materials)
{
	Dictionary<Texture2D> textures;
	return LoadMtl(filename, materials, textures);
}

bool MaterialLoader::LoadMtl(const char *filename, Dictionary<Material> &materials, Dictionary<Texture2D> &textures)
{
	ifstream file(filename);
	if (!file) return false;

	Material *current = NULL;

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

		if (prefix == "newmtl") {
			current = &materials.AddItem(line.c_str(), Material());
		}
		else if (prefix == "Ns") {
			sscanf_s(line.c_str(), "%f", &current->specularIntensity);
		}
		else if (prefix == "Ka") {
			Color3f &c = current->ambient;
			sscanf_s(line.c_str(), "%f %f %f", &c.r, &c.g, &c.b);
		}
		else if (prefix == "Kd") {
			Color3f &c = current->diffuse;
			sscanf_s(line.c_str(), "%f %f %f", &c.r, &c.g, &c.b);
		}
		else if (prefix == "Ks") {
			Color3f &c = current->specular;
			sscanf_s(line.c_str(), "%f %f %f", &c.r, &c.g, &c.b);
		}
		else if (prefix == "map_Kd") {
			current->diffuseMap = getTexture(line, textures);
		}
		else if (prefix == "map_Ks") {
			current->specularMap = getTexture(line, textures);
			current->specularMap->SetTextureUnit(GL_TEXTURE2);
		}
		else if (prefix == "map_Ns") {
			current->specularIntensityMap = getTexture(line, textures);
			current->specularIntensityMap->SetTextureUnit(GL_TEXTURE3);
		}
		else if (prefix == "map_d") {
			current->opacityMask = getTexture(line, textures);
			current->opacityMask->SetTextureUnit(GL_TEXTURE4);
		}
		else if (prefix == "map_bump" || prefix == "bump") {
			if (!current->normalMap) {
				current->normalMap = getTexture(line, textures);
				current->normalMap->SetTextureUnit(GL_TEXTURE1);
			}
		}
		else if (prefix == "illum") {
			sscanf_s(line.c_str(), "%d", &current->mode);
		}
	}

	return true;
}