#pragma once

#include <engineincl.h>

#include <engine/entity/scene.h>

namespace Techless
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(Scene* CurrentScene);
		Entity(Scene* CurrentScene, std::string newID);

		void Destroy();

		template <typename ComponentType, typename... Args>
		ComponentType& AddComponent(Args&&... args)
		{
			return ActiveScene->SceneRegistry.Add<ComponentType>(EntityID, std::forward<Args>(args)...);
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

		inline std::string& GetID() { return EntityID; };

	private:
		std::string EntityID;

		Scene* ActiveScene = nullptr;
	};
}