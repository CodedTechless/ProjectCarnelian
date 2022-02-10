#pragma once

#include <engine/entity/registry/registry.h>

#include <engine/application/event.h>
#include <engine/entity/prefabs/prefab.h>

#include <json/json.hpp>

using JSON = nlohmann::json;

namespace Techless 
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity& CreateEntity(const std::string& TagName = "Entity");
		Entity& Instantiate(Prefab& prefab);

		void Update(const float Delta, bool AllowScriptRuntime = true);
		void FixedUpdate(const float Delta);

		void Serialise(const std::string& FilePath, const Entity& RootEntity);

		inline void SetActiveCamera(Entity& entity) { ActiveCamera = &entity; };
		inline Entity& GetActiveCamera() const { return *ActiveCamera; };

		template<typename Type>
		Ptr<TypedRegistrySet<Type>> GetInstances()
		{
			return SceneRegistry.GetRegistrySet<Type>();
		}

		template<typename Type>
		Type& GetInstanceByID(const std::string& EntityID)
		{
			return SceneRegistry.Get<Type>(EntityID);
		}

		Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed);
		void OnWindowEvent(const WindowEvent& windowEvent);

	private:
		Entity* ActiveCamera = nullptr;
		void DestroyEntity(const std::string& EntityID);

		friend class Entity;

	private:
		Registry SceneRegistry;

		template<typename Component>
		void PushSerialisedComponent(JSON& j_ComponentSet, const std::unordered_map<std::string, bool>& ArchivableIndex, const std::string& EntryName)
		{
			j_ComponentSet[EntryName] = JSON::object();

			if (!SceneRegistry.HasRegistrySet<Component>())
				return;

			auto Set = SceneRegistry.GetRegistrySet<Component>();

			int i = 0;
			for (Component& component : *Set)
			{
				std::string EntityID = Set->GetIDAtIndex(i);

				if (ArchivableIndex.find(EntityID) != ArchivableIndex.end() && ArchivableIndex.at(EntityID) == true)
					j_ComponentSet.at(EntryName)[EntityID] = component;

				++i;
			}
		}

		template<typename Component>
		void AssignPrefabComponents(Prefab& prefab)
		{
			auto Components = prefab.GetComponents<Component>();
			for (auto& pair : *Components)
			{
				Entity& entity = SceneRegistry.Get<Entity>(pair.first);
				entity.AddComponent<Component>(pair.second);
			}
		}
	};
}