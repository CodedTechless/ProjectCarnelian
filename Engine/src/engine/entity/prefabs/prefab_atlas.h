#pragma once

#include <engineincl.h>

#include "prefab.h"


namespace Techless
{

	class PrefabAtlas
	{
	public:

		static Prefab& Get(const std::string& FilePath);
		static void Unload(const std::string& FilePath);

	private:

		static std::unordered_map<std::string, Prefab> Prefabs;
	};
}