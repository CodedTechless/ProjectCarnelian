#pragma once

#include <engine/entity/registry/registry.h>
#include <engine/application/event.h>

#include <json/json.hpp>

using JSON = nlohmann::json;

namespace Techless 
{
	class Entity;

	class Scene
	{
	public:
		Entity& CreateEntity(const std::string& TagName = "Entity");
		void DestroyEntity(const std::string& EntityID);
//		Entity& Instantiate();

		void Update(const float Delta, bool AllowScriptRuntime = true);
		void FixedUpdate(const float Delta);

		void Serialise(const std::string& FilePath);

		inline void SetActiveCamera(Entity& entity) { ActiveCamera = &entity; };
		inline Entity& GetActiveCamera() const { return *ActiveCamera; };

		template<typename Type>
		Ptr<TypedRegistrySet<Type>> GetInstances()
		{
			return SceneRegistry.GetRegistrySet<Type>();
		}

		Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed);
		void OnWindowEvent(const WindowEvent& windowEvent);

	private:
		Entity* ActiveCamera = nullptr;

		friend class Entity;

	private:
		Registry SceneRegistry;

		template<typename Component>
		void PushSerialisedComponent(JSON& j_ComponentSet, const std::unordered_map<std::string, bool>& ArchivableIndex, const std::string& EntryName)
		{
			j_ComponentSet[EntryName] = JSON::array();

			if (!SceneRegistry.HasRegistrySet<Component>())
				return;

			auto Set = SceneRegistry.GetRegistrySet<Component>();
			
			int i = 0;
			for (Component& component : *Set)
			{
				std::string EntityID = Set->GetIDAtIndex(i);

				if (ArchivableIndex.at(EntityID) == true)
					j_ComponentSet.at(EntryName).emplace_back(component);
			
				++i;
			}
		}
	};
}