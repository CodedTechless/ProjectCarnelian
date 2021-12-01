#include "registry.h"

namespace Techless {
	void Registry::Clear(const std::string& ID)
	{
		for (auto Set : InstanceSets)
		{
			Set.second->Clear(ID);
		}
	}
}