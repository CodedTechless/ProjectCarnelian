#pragma once


#include <engine/entity/entity.h>
#include <engine/entity/components.h>

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

#define DefineQuery(Type) if (ComponentName == #Type) return sol::make_object<Type*>(lua, QueryComponentType<Type>(Mode))

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
		LuaScriptableEntity(Ptr<Entity> entity)
			: LinkedEntity(entity) {};

		template<typename T>
		T* QueryComponentType(QueryMode Mode)
		{
			switch (Mode)
			{
			case QueryMode::Add:
			{
				if (!LinkedEntity->HasComponent<T>())
					return &LinkedEntity->AddComponent<T>();
				
				break;
			}
			case QueryMode::Remove:
				{
					if (LinkedEntity->HasComponent<T>())
						LinkedEntity->RemoveComponent<T>();
				}
			}

			return nullptr;
		}

		sol::object QueryComponent(sol::this_state s, const std::string& ComponentName, QueryMode Mode)
		{
			sol::state_view lua(s);

			// [COMPONENT ASSIGNMENT] Lua

			DefineQuery(TagComponent);
			DefineQuery(TransformComponent);
			DefineQuery(YSortComponent);
			DefineQuery(BoxColliderComponent);
//			DefineQuery(RigidBodyComponent);

			DefineQuery(SpriteComponent);
			DefineQuery(SpriteAnimatorComponent);
			DefineQuery(CameraComponent);

			DefineQuery(LuaScriptComponent);

			return sol::lua_nil;
		}

		std::string GetID() const { return LinkedEntity->GetID(); };

		void SetParent(Ptr<Entity> entity) { return LinkedEntity->SetParent(entity); };
		Ptr<Entity> GetParent() const { return LinkedEntity->GetParent(); };
		std::vector<Ptr<Entity>> GetChildren() const { return LinkedEntity->GetChildren(); };

		Ptr<Entity> GetLinkedEntity() { return LinkedEntity; };
		Ptr<Scene> GetLinkedScene() { return LinkedEntity->GetScene(); };

	private:
		Ptr<Entity> LinkedEntity;

	};

}