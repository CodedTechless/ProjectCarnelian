#include "entity.h"

#include <engine/entity/components.h>

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
		for (Entity* entity : Children)
			entity->Destroy();

		if (Parent)
			Parent->RemoveChild(this);

		ActiveScene->DestroyEntity(EntityID);
	}

	void Entity::AddChild(Entity* entity)
	{
		ScriptEnvironment::RegisterChild(ActiveScene->GetLuaID(), EntityID, entity->EntityID);

		Children.push_back(entity);
	}

	void Entity::RemoveChild(Entity* entity)
	{
		ScriptEnvironment::DeregisterChild(ActiveScene->GetLuaID(), EntityID, entity->EntityID);

		auto it = std::find(Children.begin(), Children.end(), entity);
		if (it != Children.end())
		{
			Children.erase(it);
		}
	}

	bool IsParentOfSelf(Entity* entityOriginal, Entity* entity)
	{
		if (entity == nullptr)
			return false;

		if (entityOriginal == entity)
			return true;

		return IsParentOfSelf(entityOriginal, entity->GetParent());
	}

	void Entity::SetParent(Entity* entity)
	{
		if (entity && IsParentOfSelf(this, entity))
			return;

		if (Parent != nullptr)
			Parent->RemoveChild(this);

		Parent = entity;

		if (entity != nullptr)
			entity->AddChild(this);

		if (HasComponent<TransformComponent>())
		{
			auto& Transform = GetComponent<TransformComponent>();
			Transform.MarkAsDirty();
		}
	}
}