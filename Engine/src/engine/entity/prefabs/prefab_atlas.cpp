


#include <json/json.hpp>

#include "prefab_atlas.h"


namespace Techless
{

	std::unordered_map<std::string, Prefab> PrefabAtlas::Prefabs = {};

	Prefab& PrefabAtlas::Get(const std::string& FilePath)
	{
		fs::path FullPath = { FilePath };
		std::string Name = FullPath.stem().generic_string();

		if (Prefabs.find(Name) != Prefabs.end())
			return Prefabs[Name];
		
		Debug::Log("Loaded prefab " + Name, "PrefabAtlas");
		std::ifstream i(FilePath);
		JSON json;
		i >> json;

		Prefabs[Name] = { json, Name };

		return Prefabs[Name];
	}

	void PrefabAtlas::Unload(const std::string& FilePath)
	{
		Prefabs.erase(FilePath);
	}

}