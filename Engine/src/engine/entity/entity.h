#pragma once

#include <engineincl.h>
#include <json/json.hpp>

#include <engine/lua/script_environment.h>
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

		void SetParent(Entity* entity);
		void Destroy();

		inline std::string GetID() const { return EntityID; };
		inline Scene* GetScene() const { return ActiveScene; };

		inline Entity* GetParent() const { return Parent; };
		inline std::vector<Entity*> GetChildren() const { return Children; };
		
	public:
		template <typename ComponentType, typename... Args>
		ComponentType& AddComponent(Args&&... args)
		{
			ComponentType& component = ActiveScene->SceneRegistry.Add<ComponentType>(EntityID, std::forward<Args>(args)...);
			component.LinkedEntity = this;
			
			ScriptEnvironment::RegisterComponent(ActiveScene->GetScriptEnvID(), EntityID, TYPEID_STRING(ComponentType), component);

			return component;
		}

		template <typename ComponentType>
		void RemoveComponent()
		{
			ActiveScene->SceneRegistry.Remove<ComponentType>(EntityID);
			ScriptEnvironment::DeregisterComponent(ActiveScene->GetScriptEnvID(), EntityID, TYPEID_STRING(ComponentType));
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

		void AddChild(Entity* entity);
		void RemoveChild(Entity* entity);

		Entity* Parent = nullptr;
		std::vector<Entity*> Children{};

	public: // json serialisation
		bool Archivable = true;

		inline friend void to_json(JSON& json, const Entity& entity)
		{
			json = JSON{
				{"EntityID", entity.EntityID}
			};

			if (entity.Parent)
				json["ParentEntityID"] = entity.Parent->EntityID;
			else
				json["ParentEntityID"] = nullptr;
		}

		inline friend void from_json(const JSON& json, Entity& entity)
		{
			json.at("EntityID").get_to(entity.EntityID);
		}

	};
}