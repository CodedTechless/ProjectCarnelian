#pragma once

#include <engineincl.h>
#include <json/json.hpp>

#include <engine/entity/scene.h>

using JSON = nlohmann::json;

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

		inline std::string GetID() const { return EntityID; };
		inline Scene* GetScene() const { return ActiveScene; };

	private:
		std::string EntityID;

		Scene* ActiveScene = nullptr;
	
	public:

		inline friend void to_json(JSON& json, const Entity& entity)
		{
			json = JSON{
				{"EntityID", entity.EntityID}
			};
		}

		inline friend void from_json(const JSON& json, Entity& entity)
		{
			json.at("EntityID").get_to(entity.EntityID);
		}
	};
}