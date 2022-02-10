#include "prefab.h"

#include <engine/entity/component/components.h>

namespace Techless
{

	Prefab::Prefab(JSON& json, const std::string& filePath)
	{
		FilePath = filePath;
		Deserialise(json);
	}

	void Prefab::Deserialise(JSON& json)
	{
		JSON& j_Entities = json["Entities"];
		JSON& j_Components = json["Components"];

		std::string RootEntityID = json["Scene"]["RootEntityID"].get<std::string>();

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
	}

}