#include "material.h"
#include <utility>

Texture2D MaterialLib::getTexture(const string &name)
{
	typedef std::map<string, Texture2D> tex_map;

	tex_map::iterator i = rc->textures.find(name);
	if (i != rc->textures.end()) {
		return i->second;
	}

	Texture2D tex(name.c_str());
	rc->textures.insert(make_pair(name, tex));
	return tex;
}

bool MaterialLib::LoadMtl(const char *filename)
{
	ifstream file(filename);
	if (!file) return false;

	map.clear();
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
			current = &(map[line] = Material());
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
			current->diffuseMap = getTexture(line);
		}
		else if (prefix == "map_Ks") {
			current->specularMap = getTexture(line);
			current->specularMap->SetTextureUnit(GL_TEXTURE3);
		}
		else if (prefix == "map_Ns") {
			current->specularIntensityMap = getTexture(line);
			current->specularIntensityMap->SetTextureUnit(GL_TEXTURE2);
		}
		else if (prefix == "map_bump" || prefix == "bump") {
			if (!current->normalMap) {
				current->normalMap = getTexture(line);
				current->normalMap->SetTextureUnit(GL_TEXTURE1);
			}
		}
		else if (prefix == "illum") {
			sscanf_s(line.c_str(), "%d", &current->mode);
		}
	}

	return true;
}