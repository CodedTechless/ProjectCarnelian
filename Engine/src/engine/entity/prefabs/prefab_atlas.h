#pragma once


#include <engineincl.h>

#include "prefab.h"

namespace Techless
{

	class PrefabAtlas
	{
	public:

		Ptr<Prefab> Load(const std::string& FilePath);
	
	private:
	};
}