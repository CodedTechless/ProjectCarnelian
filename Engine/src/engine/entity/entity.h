#pragma once

#include <engineincl.h>

#include <engine/lua/script_environment.h>
#include <engine/entity/scene.h>

namespace Techless
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(Scene* CurrentScene);
		Entity(Scene* CurrentScene, std::string newID);

		void SetParent(Ptr<Entity> entity);
		void Destroy();

		inline std::string GetID() const { return EntityID; };
		inline Scene* GetScene() const { return ActiveScene; };

		inline Ptr<Entity> GetParent() const { return Parent; };
		inline std::vector<Ptr<Entity>>& GetChildren() { return Children; };
		
	public:
		template <typename ComponentType, typename... Args>
		ComponentType& AddComponent(Args&&... args)
		{
			ComponentType& component = ActiveScene->SceneRegistry.Add<ComponentType>(EntityID, std::forward<Args>(args)...);
			component.LinkedEntity = ActiveScene->GetEntityByID(EntityID);

			return component;
		}

		template <typename ComponentType>
		void RemoveComponent()
		{
			ActiveScene->SceneRegistry.Remove<ComponentType>(EntityID);
		}

		template <typename ComponentType>
		ComponentType& GetComponent()
		{
			return ActiveScene->SceneRegistry.Get<ComponentType>(EntityID);
		}

		template <typename ComponentType>
		bool HasComponent()
		{
			return ActiveScene->SceneRegistry.Has<ComponentType>(EntityID);
		}

	private:
		std::string EntityID;
		Scene* ActiveScene = nullptr;

		void AddChild(const std::string& ChildID);
		void RemoveChild(const std::string& ChildID);
		bool IsParentOfSelf(Ptr<Entity> PossibleChildEntity);

		Ptr<Entity> Parent = nullptr;
		std::vector<Ptr<Entity>> Children = {};

		friend class Scene;

	public:
		bool Archivable = true;

	};
}