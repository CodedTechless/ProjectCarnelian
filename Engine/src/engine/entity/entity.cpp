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
		while (Children.size() != 0)
		{
			Children[0]->Destroy();
		}

		if (Parent)
			Parent->RemoveChild(EntityID);

		ActiveScene->DestroyEntity(EntityID);
	}

	void Entity::AddChild(const std::string& ChildID)
	{
		Ptr<Entity> Child = ActiveScene->GetEntityByID(ChildID);
		ScriptEnvironment::RegisterChild(ActiveScene->GetLuaID(), EntityID, ChildID);

		Children.push_back(Child);
	}

	void Entity::RemoveChild(const std::string& ChildID)
	{
		ScriptEnvironment::DeregisterChild(ActiveScene->GetLuaID(), EntityID, ChildID);

		auto it = std::find_if(Children.begin(), Children.end(), 
			[&](Ptr<Entity> ent) 
			{ 
				return ent->GetID() == ChildID;
			}
		);

		if (it != Children.end())
		{
			Children.erase(it);
		}
	}

	bool Entity::IsParentOfSelf(Ptr<Entity> PossibleChildEntity)
	{
		if (PossibleChildEntity == nullptr)
			return false;

		if (EntityID == PossibleChildEntity->EntityID)
			return true;

		return IsParentOfSelf(PossibleChildEntity->Parent);
	}

	void Entity::SetParent(Ptr<Entity> NewParent)
	{
		if (NewParent && IsParentOfSelf(NewParent))
			return;

		if (Parent != nullptr)
			Parent->RemoveChild(EntityID);

		Parent = NewParent;

		if (NewParent != nullptr)
			NewParent->AddChild(EntityID);

		if (HasComponent<TransformComponent>())
		{
			auto& Transform = GetComponent<TransformComponent>();
			Transform.MarkTransformationDirty();
		}
	}
}