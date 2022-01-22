#include "scene.h"

#include <engine/entity/component/components.h>
#include <engine/entity/entity.h>
#include <uuid/uuid.hpp>

#include <render/renderer.h>

namespace Techless
{

	Entity& Scene::CreateEntity(const std::string& TagName)
	{
		std::string NewUUID = UUID::Generate();

		auto& Ent = SceneRegistry.Add<Entity>(NewUUID, this, NewUUID);
		Ent.AddComponent<TransformComponent>();

		auto& Tag = Ent.AddComponent<TagComponent>();
		Tag.Name = TagName;

		return Ent;
	}

	void Scene::DestroyEntity(const std::string& EntityID)
	{
		if (SceneRegistry.Has<ScriptComponent>(EntityID))
		{
			auto& Script = SceneRegistry.Get<ScriptComponent>(EntityID);
			Script.Instance->OnDestroy();
		}

		SceneRegistry.Remove<Entity>(EntityID);
	}

	Entity& Scene::Instantiate(const Prefab& prefab)
	{

	}

	Input::Filter Scene::OnInputEvent(const InputEvent& inputEvent, bool Processed)
	{
		/*
			to-do:
			 - make it so inputs propagate from positive to negative depths
			 - do it efficiently (don't sort on z-depth every input because that's really fucking inefficient bitch)
		*/

		auto FinalFilter = Input::Filter::Ignore;
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (auto Script : *ScriptComponents)
		{
			auto Response = Script.Instance->OnInputEvent(inputEvent, Processed);

			if (Response != Input::Filter::Ignore)
				FinalFilter = Response;

			if (Response == Input::Filter::Stop)
				break;
			else if (Response == Input::Filter::Continue)
				Processed = true;
		}

		return FinalFilter;
	}

	void Scene::OnWindowEvent(const WindowEvent& windowEvent)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (auto Script : *ScriptComponents)
		{
			Script.Instance->OnWindowEvent(windowEvent);
		}
	}

	void Scene::FixedUpdate(const float Delta)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		for (auto Script : *ScriptComponents)
		{
			Script.Instance->OnFixedUpdate(Delta);
		}
	}

	void Scene::Update(const float Delta, bool AllowScriptRuntime)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		if (AllowScriptRuntime)
		{
			for (auto& Script : *ScriptComponents)
			{
				Script.Instance->OnUpdate(Delta);
			}
		}

		// Sprite Rendering
		
		if (!ActiveCamera) return;

		CameraComponent& CameraComp = ActiveCamera->GetComponent<CameraComponent>();
		TransformComponent& CameraTransformComp = ActiveCamera->GetComponent<TransformComponent>();
		glm::vec3 CameraPosition = CameraTransformComp.GetGlobalPosition();
		
		glm::mat4 CameraProjection = CameraComp.GetProjection();
		glm::mat4 CameraTransform = CameraComp.GetTransform(CameraPosition);
		
		Renderer::Begin(CameraProjection, CameraTransform);

		if (AllowScriptRuntime)
		{
			for (auto& Script : *ScriptComponents)
			{
				Script.Instance->OnDraw(Delta);
			}
		}

		{
			auto SpriteComponents = SceneRegistry.GetRegistrySet<SpriteComponent>();
			auto TransformComponents = SceneRegistry.GetRegistrySet<TransformComponent>();

			unsigned int i = 0;
			for (auto& Sprite : *SpriteComponents)
			{
				auto EntityID = SpriteComponents->GetIDAtIndex(i);
				auto& Transform = TransformComponents->Get(EntityID);

				auto aSprite = Sprite.GetSprite();
				Renderer::DrawSprite(aSprite, Transform.GetGlobalTransform(), Sprite.SpriteColour);

				++i;
			}
		}

		Renderer::End();
	}

	void Scene::Serialise(const std::string& FilePath)
	{
		JSON j_SerialisedScene = {
			{"Entities", JSON::array()},
			{"Components", JSON::object()}
		};

		JSON& j_Entities = j_SerialisedScene.at("Entities");
		JSON& j_Components = j_SerialisedScene.at("Components");

		std::unordered_map<std::string, bool> ArchivableIndex = {};

		auto Entities = SceneRegistry.GetRegistrySet<Entity>();
		for (auto& entity : *Entities)
		{
			ArchivableIndex[entity.GetID()] = entity.Archivable;

			if (!entity.Archivable)
				continue;

			j_Entities += entity;
		}

		PushSerialisedComponent <TagComponent>			(j_Components, ArchivableIndex, "Tag");
		PushSerialisedComponent <TransformComponent>	(j_Components, ArchivableIndex, "Transform");
		PushSerialisedComponent <RigidBodyComponent>	(j_Components, ArchivableIndex, "RigidBody");
		PushSerialisedComponent <SpriteComponent>		(j_Components, ArchivableIndex, "Sprite");
//		PushSerialisedComponent <AnimatorComponent>		(j_Components, ArchivableIndex, "Animator");
		PushSerialisedComponent <CameraComponent>		(j_Components, ArchivableIndex, "Camera");

		std::ofstream o(FilePath);
		o << j_SerialisedScene << std::endl;
	}
}