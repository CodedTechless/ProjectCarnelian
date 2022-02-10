


#include <json/json.hpp>

#include "prefab_atlas.h"

namespace Techless
{

	std::unordered_map<std::string, Prefab> PrefabAtlas::Prefabs = {};

	Prefab& PrefabAtlas::Get(const std::string& FilePath)
	{
		if (Prefabs.find(FilePath) != Prefabs.end())
			return Prefabs[FilePath];
		
		std::ifstream i(FilePath);
		JSON json;
		i >> json;

		Prefabs[FilePath] = { json, FilePath };
		Debug::Log("Loaded prefab " + FilePath, "PrefabAtlas");

		return Prefabs[FilePath];
	}

	void PrefabAtlas::Unload(const std::string& FilePath)
	{
		Prefabs.erase(FilePath);
	}

}