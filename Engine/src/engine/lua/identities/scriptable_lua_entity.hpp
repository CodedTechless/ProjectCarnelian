#pragma once


#include <engine/entity/entity.h>
#include <engine/entity/component/components.h>

#include <engineincl.h>

/*
lua does:
 - request add components
 - request remove components
 - caches pointers to components on its own end so it can alter their properties

c++ does:
 - pushes component pointers to lua
 - tell lua when an entity or a component gets removed so lua can uncache them
*/

#define PullQuery(Name) "QueryComponent_" #Name , &LuaScriptableEntity::QueryComponent_##Name

#define DefineQuery(Name, Type) \
	Type* QueryComponent_##Name(QueryMode Mode)\
	{\
		switch (Mode)\
		{\
			case QueryMode::Add:\
			{\
				if (!LinkedEntity->HasComponent<Type>())\
					return &LinkedEntity->AddComponent<Type>();\
				break;\
			}\
			case QueryMode::Remove:\
			{\
				if (LinkedEntity->HasComponent<Type>())\
					LinkedEntity->RemoveComponent<Type>();\
			}\
		}\
		return nullptr;\
	}

namespace Techless
{

	enum class QueryMode
	{
		Add,
		Remove
	};

	class LuaScriptableEntity
	{
	public:
		LuaScriptableEntity(Entity* entity)
			: LinkedEntity(entity) {};

		// [COMPONENT ASSIGNMENT]

		DefineQuery(Tag, TagComponent);
		DefineQuery(Transform, TransformComponent);
		DefineQuery(RigidBody, RigidBodyComponent);
		DefineQuery(Sprite, SpriteComponent);
		DefineQuery(Camera, CameraComponent);

		std::string GetID() const { return LinkedEntity->GetID(); };

		void SetParent(Entity* entity) { return LinkedEntity->SetParent(entity); };
		Entity* GetParent() const { return LinkedEntity->GetParent(); };
		std::vector<Entity*> GetChildren() const { return LinkedEntity->GetChildren(); };

		Entity* GetLinkedEntity() { return LinkedEntity; };
		Scene* GetLinkedScene() { return LinkedEntity->GetScene(); };

	private:
		Entity* LinkedEntity;

	};

}