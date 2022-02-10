

#include "registry.h"

namespace Techless 
{
	
	void Registry::Clear(std::string ID)
	{
		for (auto Set : InstanceSets)
		{
			Set.second->Clear(ID);
		}
	}

}