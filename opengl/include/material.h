#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include <map>
#include "nullable.h"
#include "datatypes.h"
#include "texture.h"

using namespace std;

class GLRenderingContext;
template<class T> class Dictionary;

enum MaterialMode
{
	MM_NO_LIGHTING = 0,
	MM_LAMBERT = 1,
	MM_BLINN_PHONG = 2
};

class Material
{
public:
	Color4f ambient;
	Color4f diffuse;
	Color4f specular;
	float specularIntensity;
	Nullable<Texture2D> diffuseMap;
	Nullable<Texture2D> normalMap;
	Nullable<Texture2D> specularMap;
	Nullable<Texture2D> specularIntensityMap;
	Nullable<Texture2D> opacityMask;
	MaterialMode mode;

	Material() {
		specularIntensity = 0.0f;
		mode = MM_NO_LIGHTING;
	}
};

class MaterialLoader
{
public:
	bool LoadMtl(const char *filename, Dictionary<Material> &materials);
	bool LoadMtl(const char *filename, Dictionary<Material> &materials, Dictionary<Texture2D> &textures);
private:
	Texture2D getTexture(const string &name, Dictionary<Texture2D> &textures);
};

template<class T>
class Dictionary
{
public:
	typedef T mapped_type;

	T *GetItem(const char *name) {
		map_t::iterator i = map.find(name);
		return i != map.end() ? &i->second : NULL;
	}

	const T *GetItem(const char *name) const {
		map_t::const_iterator i = map.find(name);
		return i != map.end() ? &i->second : NULL;
	}

	T &AddItem(const char *name, const T &mat) {
		return map.insert(make_pair(name, mat)).first->second;
	}

	bool HasItem(const char *name) const {
		return map.find(name) != map.end();
	}

	void RemoveItem(const char *name) {
		map.erase(name);
	}

	Material *operator[](const char *name) {
		return GetItem(name);
	}
protected:
	typedef map<string, T> map_t;
	map_t map;
};

template<class T>
class LibCollection
{
public:
	LibCollection() {
		defaultLib = "default";
		AddLib("default", Dictionary<T>());
	}

	bool SetDefaultLib(const char *libName) {
		if (GetLib(libName)) {
			defaultLib = libName;
			return true;
		}
		return false;
	}

	Dictionary<T> &GetDefaultLib() {
		return *GetLib(defaultLib.c_str());
	}

	typename T *GetItem(const char *itemName) {
		return GetItem(defaultLib.c_str(), itemName);
	}

	typename T *GetItem(const char *libName, const char *itemName)
	{
		Dictionary<T> *lib = dicts.GetItem(libName);
		if (!lib) return NULL;
		return lib->GetItem(itemName);
	}

	Dictionary<T> &AddLib(const char *name, const Dictionary<T> &lib) {
		return dicts.AddItem(name, lib);
	}

	Dictionary<T> *GetLib(const char *name) {
		return dicts.GetItem(name);
	}

	void RemoveLib(const char *name) {
		dicts.RemoveItem(name);
	}

	typename T *operator()(const char *libName, const char *itemName)
	{
		return GetItem(libName, itemName);
	}
private:
	Dictionary<Dictionary<T>> dicts;
	string defaultLib;
};

#endif // _MATERIAL_H_