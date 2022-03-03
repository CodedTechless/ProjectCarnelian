


#include <json/json.hpp>

#include <engine/entity/serialiser/serialiser.h>

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

		Deserialiser deserialiser = { FilePath };
		Prefabs[Name] = deserialiser.Deserialise();

		Debug::Log("Loaded prefab " + Name, "PrefabAtlas");

		return Prefabs[Name];
	}

	void PrefabAtlas::Unload(const std::string& FilePath)
	{
		Prefabs.erase(FilePath);
	}

}