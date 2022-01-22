#include "prefab.h"


namespace Techless
{

	Prefab::Prefab(const JSON& json)
	{
		Deserialise(json);
	}

	void Prefab::Deserialise(const JSON& json)
	{
		
		auto& Entities = json["Entities"];
		auto& Components = json["Components"];

		GetSerialisedComponent
	}

}