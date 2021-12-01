#include "entity.h"

#include <uuid/uuid.hpp>



namespace Techless
{
	Entity::Entity(Scene* CurrentScene)
		: ActiveScene(CurrentScene)
	{
		EntityID = UUID::Generate();
	}

	Entity::Entity(Scene* CurrentScene, std::string newID)
		: EntityID(newID), ActiveScene(CurrentScene) {};

	void Entity::Destroy()
	{
		ActiveScene->SceneRegistry.Clear(EntityID);
		ActiveScene->DestroyEntity(EntityID);
	}
}