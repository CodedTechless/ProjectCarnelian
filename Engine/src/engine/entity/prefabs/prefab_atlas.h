#pragma once


#include <engineincl.h>

#include "prefab.h"

namespace Techless
{

	class PrefabAtlas
	{
	public:

		static Ptr<Prefab> Get(const std::string& FilePath);
	
	private:

		static std::unordered_map<std::string, Ptr<Prefab>> Prefabs;
	};
}