#include "prefab.h"

#include <engine/entity/components.h>

namespace Techless
{

	Prefab::Prefab(/*JSON& json,*/ const std::string& name)
		: Name(name)
	{
//		Deserialise(json);
	}

	/*
	void Prefab::Deserialise(JSON& json)
	{
		JSON& j_Entities = json["Entities"];
		JSON& j_Components = json["Components"];

		std::string RootEntityID = json["Scene"]["RootEntityID"].get<std::string>();

		Entities.resize(j_Entities.size());

		for (auto& v : j_Entities)
		{
			PrefabEntity ent = v.get<PrefabEntity>();
		
			if (ent.EntityID == RootEntityID)
				ent.IsRoot = true;

			
			Entities.push_back(ent);
		}

		// [COMPONENT ASSIGNMENT]

		PullSerialisedComponents<TagComponent>			(j_Components, "Tag");
		PullSerialisedComponents<TransformComponent>	(j_Components, "Transform");
		PullSerialisedComponents<RigidBodyComponent>	(j_Components, "RigidBody");
		PullSerialisedComponents<SpriteComponent>		(j_Components, "Sprite");
		PullSerialisedComponents<CameraComponent>		(j_Components, "Camera");
		PullSerialisedComponents<LuaScriptComponent>	(j_Components, "LuaScript");

		Loaded = true;
	}
	*/
}