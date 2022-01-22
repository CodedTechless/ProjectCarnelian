


#include <json/json.hpp>

#include "prefab_atlas.h"

namespace Techless
{


	Ptr<Prefab> PrefabAtlas::Get(const std::string& FilePath)
	{

		if (Prefabs.find(FilePath) != Prefabs.end())
			return Prefabs[FilePath];



		
		return nullptr;
	}

}