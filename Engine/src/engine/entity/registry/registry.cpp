

#include "registry.h"

#include <engine/entity/entity.h>

namespace Techless 
{
	
	void Registry::Clear(std::string ID)
	{
		for (auto Set : InstanceSets)
		{
			if (std::string(Set.first) != TYPEID_STRING(Entity))
			{
				Set.second->Clear(ID);
			}
		}

		GetRegistrySet<Entity>()->Clear(ID);
	}

}