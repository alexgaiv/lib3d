#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "datatypes.h"
#include "glcontext.h"
#include "texture.h"
#include "nullable.h"
#include <map>

using namespace std;

class Texture2D;
class MaterialLib;
class GLRenderingContext;

enum MaterialMode
{
	MM_CONST_COLOR = 0,
	MM_LAMBERT = 1,
	MM_BLINN_PHONG = 2
};

class Material
{
public:
	float specularIntensity;
	Color3f ambient;
	Color3f diffuse;
	Color3f specular;
	Nullable<Texture2D> diffuseMap;
	Nullable<Texture2D> normalMap;
	Nullable<Texture2D> specularMap;
	Nullable<Texture2D> specularIntensityMap;
	MaterialMode mode;

	Material() {
		specularIntensity = 0.0f;
		mode = MM_CONST_COLOR;
	}
};

class MaterialLib
{
public:
	MaterialLib(GLRenderingContext *rc) : rc(rc) { }
	bool LoadMtl(const char *filename);
	
	Material *GetMaterial(const char *name) {
		map_t::iterator i = map.find(name);
		return i != map.end() ? &i->second : NULL;
	}

	const Material *GetMaterial(const char *name) const {
		map_t::const_iterator i = map.find(name);
		return i != map.end() ? &i->second : NULL;
	}

	void AddMaterial(const char *name, const Material &mat) {
		map[name] = mat;
	}

	bool HasMaterial(const char *name) const {
		return map.find(name) != map.end();
	}
private:
	GLRenderingContext *rc;
	typedef map<string, Material> map_t;
	map_t map;

	Texture2D getTexture(const string &name);
};

#endif // _MATERIAL_H_