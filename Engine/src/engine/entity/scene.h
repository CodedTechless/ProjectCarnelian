#pragma once

#include <engine/entity/registry/registry.h>

#include <engine/application/event.h>
#include <engine/entity/prefabs/prefab.h>

namespace Techless 
{
	
	class Entity;

	class Scene
	{
	public:
		static Ptr<Scene> Create();

	public:
		Scene() = default;
		~Scene() = default;

		void Update(const float Delta);
		void FixedUpdate(const float Delta);

		Input::Filter OnInputEvent(InputEvent inputEvent, bool Processed);
		void OnWindowEvent(const WindowEvent& windowEvent);

	public:
		void Serialise(const std::string& FilePath, Ptr<Entity> RootEntity);

		Ptr<Entity> CreateBlankEntity();
		Ptr<Entity> CreateEntity(const std::string& TagName = "Entity");

//		Entity& DuplicateEntity(Entity& entity, Entity* parent);
		Ptr<Entity> Instantiate(Prefab& prefab);

		Ptr<Entity> GetEntityByID(const std::string& EntityID);
		std::vector<Ptr<Entity>>& GetEntities() { return Entities; };

		void SetActiveCamera(Ptr<Entity> entity) { ActiveCamera = entity; };
		Ptr<Entity> GetActiveCamera() const { return ActiveCamera; };

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

	public:
		void SetScriptExecutionEnabled(bool Mode) { FLAG_ScriptExecutionEnabled = Mode; };
		inline bool IsScriptExecutionEnabled() const { return FLAG_ScriptExecutionEnabled; };
		
		inline int GetLuaID() const { return SceneLuaID; };

	private:
		Ptr<Entity> ActiveCamera = nullptr;

		Registry SceneRegistry;
		std::vector<Ptr<Entity>> Entities = {};

		void DestroyEntity(const std::string& EntityID);

	private:
		int SceneLuaID = 0;
		bool FLAG_ScriptExecutionEnabled = true;

		friend class Entity;
	};
}