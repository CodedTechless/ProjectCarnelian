#include "scene.h"

#include <engine/application/application.h>

#include <engine/entity/components.h>
#include <engine/entity/entity.h>
#include <engine/entity/prefabs/prefab.h>
#include <engine/entity/serialiser/serialiser.h>

#include <engine/lua/script_environment.h>

#include <render/renderer.h>

#include <uuid/uuid.hpp>

namespace Techless
{




	// Creates a Scene object (a shared_ptr) which has a pre-registered Lua scene counter-part.
	Ptr<Scene> Scene::Create()
	{
		Ptr<Scene> newScene = CreatePtr<Scene>();
		newScene->SceneLuaID = ScriptEnvironment::RegisterScene(newScene);

		return newScene;
	}

	Entity& Scene::CreateEntity()
	{
		std::string NewUUID = UUID::Generate();

		Entity& Ent = SceneRegistry.Add<Entity>(NewUUID, this, NewUUID);
		ScriptEnvironment::RegisterEntity(SceneLuaID, &Ent);

		return Ent;
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

	Entity& Scene::DuplicateEntity(Entity& entity, Entity* parent)
	{

	}

	template<typename T>
	static void AssignComponents(Scene& scene, Prefab& prefab)
	{

	}

	Entity& Scene::Instantiate(Prefab& prefab)
	{
		// to-do: add some sort of checklist for colliding uuids?!?! hello?? [CRITICAL BUG]
		
		std::vector<Entity*> Entities = {};

		for (uint16_t i = 0; i < prefab.Entities; ++i)
		{
			
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
		SceneRegistry.View<TransformComponent>(
			[](TransformComponent& Transform)
			{
				if (Transform.IsEngineInterpolationEnabled())
					Transform.ForceReloadPreviousState();
			});

		SceneRegistry.View<ScriptComponent>(
			[&](ScriptComponent& Script)
			{
				Script.Instance->OnFixedUpdate(Delta);
			});

		ScriptEnvironment::CallScene(SceneLuaID, "OnFixedUpdate", Delta);
	}


	struct SceneRenderInfo
	{
		SpriteComponent* SpritePtr = nullptr;
		TransformComponent* TransformPtr = nullptr;
	};

	void Scene::Update(float Delta, bool AllowScriptRuntime)
	{
		if (AllowScriptRuntime)
		{
			SceneRegistry.View<ScriptComponent>(
				[=](ScriptComponent& Script)
				{
					Script.Instance->OnUpdate(Delta);
				});

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
			std::vector<SceneRenderInfo> SceneSprites{};

			SceneRegistry.View<SpriteComponent, TransformComponent>(
				[&](SpriteComponent& c_Sprite, TransformComponent& c_Transform)
				{
					Ptr<Sprite> aSprite = c_Sprite.GetSprite();
					if (aSprite)
						SceneSprites.push_back({ &c_Sprite, &c_Transform});

				});

			std::sort(SceneSprites.begin(), SceneSprites.end(), [](SceneRenderInfo& a, SceneRenderInfo& b)
				{
					return a.TransformPtr->GetGlobalPosition().z < b.TransformPtr->GetGlobalPosition().z;
				});

			for (SceneRenderInfo& renderInfo : SceneSprites)
			{
				Renderer::DrawSprite(renderInfo.SpritePtr->GetSprite(), renderInfo.TransformPtr->GetGlobalTransform(), renderInfo.SpritePtr->SpriteColour);
			}
		}

		Renderer::End();
	}

	void Scene::Serialise(const std::string& FilePath, Entity& RootEntity)
	{
		Serialiser l_Serialiser = { RootEntity };
		l_Serialiser.SaveToFile(FilePath);
	}
}