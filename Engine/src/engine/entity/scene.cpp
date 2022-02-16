#include "scene.h"

#include <engine/entity/component/components.h>
#include <engine/entity/entity.h>
#include <engine/entity/prefabs/prefab.h>
#include <uuid/uuid.hpp>

#include <engine/lua/script_environment.h>

#include <render/renderer.h>

namespace Techless
{

	// Creates a Scene object (a shared_ptr) which has a pre-registered Lua scene counter-part.
	Ptr<Scene> Scene::Create()
	{
		Ptr<Scene> newScene = CreatePtr<Scene>();
		newScene->SceneLuaID = ScriptEnvironment::RegisterScene(newScene);

		return newScene;
	}

	Entity& Scene::CreateEntity(const std::string& TagName)
	{

		std::string NewUUID = UUID::Generate();

		Entity& Ent = SceneRegistry.Add<Entity>(NewUUID, this, NewUUID);
		ScriptEnvironment::RegisterEntity(SceneLuaID, &Ent);

		Ent.AddComponent<TransformComponent>();

		auto& Tag = Ent.AddComponent<TagComponent>();
		Tag.Name = TagName;

		return Ent;
	}

	void Scene::DestroyEntity(const std::string& EntityID)
	{
		ScriptEnvironment::DeregisterEntity(SceneLuaID, EntityID);

		if (SceneRegistry.Has<ScriptComponent>(EntityID))
		{
			auto& Script = SceneRegistry.Get<ScriptComponent>(EntityID);
			Script.Instance->OnDestroy();
		}

		SceneRegistry.Clear(EntityID);
	}

	Entity& Scene::Instantiate(Prefab& prefab)
	{
		// to-do: add some sort of checklist for colliding uuids?!?! hello??
		// the chances are HORRIFICALLY LOW for a collision, but "HORRIFICALLY LOW" =/= none!!
		
		Entity* RootEntity = nullptr;

		// Create all the prefab entities with their respective ID's
		for (const PrefabEntity& prefabEntity : prefab.Entities)
		{
			Entity& newEntity = SceneRegistry.Add<Entity>(prefabEntity.EntityID, this, prefabEntity.EntityID);
			ScriptEnvironment::RegisterEntity(SceneLuaID, &newEntity);

			if (prefabEntity.IsRoot)
				RootEntity = &newEntity;
		}

		// Set all of the parents of the prefab object
		auto Entities = SceneRegistry.GetRegistrySet<Entity>();
		for (const PrefabEntity& prefabEntity : prefab.Entities)
		{
			Entity& newEntity = Entities->Get(prefabEntity.EntityID);

			if (prefabEntity.ParentEntityID != "")
				newEntity.SetParent(&Entities->Get(prefabEntity.ParentEntityID));
		}

		// [COMPONENT ASSIGNMENT]
		// Assign all of the relevant components!
		AssignPrefabComponents<TagComponent>		(prefab);
		AssignPrefabComponents<TransformComponent>	(prefab);
		AssignPrefabComponents<RigidBodyComponent>	(prefab);
		AssignPrefabComponents<SpriteComponent>		(prefab);
		AssignPrefabComponents<CameraComponent>		(prefab);
		AssignPrefabComponents<LuaScriptComponent>	(prefab);

		return *RootEntity;
	}

	Input::Filter Scene::OnInputEvent(const InputEvent& inputEvent, bool Processed)
	{
		/*
			to-do: (list)
			 - make it so inputs propagate from positive to negative depths
			 - do it efficiently (don't sort on z-depth every input because that's really fucking inefficient bitch)
		*/

		Input::Filter FinalFilter = Input::Filter::Ignore;
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (ScriptComponent& Script : *ScriptComponents)
		{
			Input::Filter Response = Script.Instance->OnInputEvent(inputEvent, Processed);

			if (Response != Input::Filter::Ignore)
				FinalFilter = Response;

			if (Response == Input::Filter::Stop)
				break;
			else if (Response == Input::Filter::Continue)
				Processed = true;
		}

		if (FinalFilter != Input::Filter::Stop)
		{
			Input::Filter LuaFilter = ScriptEnvironment::CallScene(SceneLuaID, "OnInputEvent", inputEvent, Processed).as<Input::Filter>();
		}

		return FinalFilter;
	}

	void Scene::OnWindowEvent(const WindowEvent& windowEvent)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (auto& Script : *ScriptComponents)
		{
			Script.Instance->OnWindowEvent(windowEvent);
		}

		ScriptEnvironment::CallScene(SceneLuaID, "OnWindowEvent", windowEvent);
	}

	void Scene::FixedUpdate(float Delta)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (auto& Script : *ScriptComponents)
		{
			Script.Instance->OnFixedUpdate(Delta);
		}

		ScriptEnvironment::CallScene(SceneLuaID, "OnFixedUpdate", Delta);
	}

	void Scene::Update(float Delta, bool AllowScriptRuntime)
	{
		if (AllowScriptRuntime)
		{
			auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

			for (auto& Script : *ScriptComponents)
			{
				Script.Instance->OnUpdate(Delta);
			}

			ScriptEnvironment::CallScene(SceneLuaID, "OnUpdate", Delta);
		}

		// Sprite Rendering
		
		if (!ActiveCamera) return;

		CameraComponent& CameraComp = ActiveCamera->GetComponent<CameraComponent>();
		TransformComponent& CameraTransformComp = ActiveCamera->GetComponent<TransformComponent>();
		glm::vec3 CameraPosition = CameraTransformComp.GetGlobalPosition();
		
		glm::mat4 CameraProjection = CameraComp.GetProjection();
		glm::mat4 CameraTransform = CameraComp.GetTransform(CameraPosition);
		
		Renderer::Begin(CameraProjection, CameraTransform);

		{
			auto SpriteComponents = SceneRegistry.GetRegistrySet<SpriteComponent>();
			auto TransformComponents = SceneRegistry.GetRegistrySet<TransformComponent>();

			unsigned int i = 0;
			for (auto& Sprite : *SpriteComponents)
			{
				auto EntityID = SpriteComponents->GetIDAtIndex(i);
				auto& Transform = TransformComponents->Get(EntityID);

				auto aSprite = Sprite.GetSprite();
				if (aSprite)
					Renderer::DrawSprite(aSprite, Transform.GetGlobalTransform(), Sprite.SpriteColour);

				++i;
			}
		}

		Renderer::End();
	}

	static void PushSerialisedChildren(JSON& j_Entities, std::unordered_map<std::string, bool>& ArchivableIndex, const Entity& entity)
	{
		if (!entity.Archivable)
			return;
		
		ArchivableIndex[entity.GetID()] = true;
		j_Entities += entity;
		
		for (Entity* child : entity.GetChildren())
		{
			PushSerialisedChildren(j_Entities, ArchivableIndex, *child);
		}
	}

	void Scene::Serialise(const std::string& FilePath, const Entity& RootEntity)
	{
		JSON j_SerialisedScene = {
			{"Scene", {
				{"RootEntityID", RootEntity.GetID()}
			}},
			{"Entities", JSON::array()},
			{"Components", JSON::object()}
		};

		JSON& j_Entities = j_SerialisedScene.at("Entities");
		JSON& j_Components = j_SerialisedScene.at("Components");

		std::unordered_map<std::string, bool> ArchivableIndex = {};

		PushSerialisedChildren(j_Entities, ArchivableIndex, RootEntity);

		// to-do: make this system better in some way (combine pull/push?)
		// this works for now :)
		// would be nice if i didnt have to assign component types in three different locations but yknoww, it's cooool-

		// [COMPONENT ASSIGNMENT] Prefab Serialisation

		PushSerialisedComponent <TagComponent>			(j_Components, ArchivableIndex, "Tag");
		PushSerialisedComponent <TransformComponent>	(j_Components, ArchivableIndex, "Transform");
		PushSerialisedComponent <RigidBodyComponent>	(j_Components, ArchivableIndex, "RigidBody");
		PushSerialisedComponent <SpriteComponent>		(j_Components, ArchivableIndex, "Sprite");
//		PushSerialisedComponent <AnimatorComponent>		(j_Components, ArchivableIndex);
		PushSerialisedComponent <CameraComponent>		(j_Components, ArchivableIndex, "Camera");
		PushSerialisedComponent <LuaScriptComponent>	(j_Components, ArchivableIndex, "LuaScript");

		std::ofstream o(FilePath);
		o << j_SerialisedScene << std::endl;
	}
}