#include "scene.h"

#include <engine/entity/component/components.h>
#include <engine/entity/entity.h>
#include <uuid/uuid.hpp>

#include <render/renderer.h>

namespace Techless
{

	Entity& Scene::CreateEntity()
	{
		std::string NewUUID = UUID::Generate();
		return SceneRegistry.Add<Entity>(NewUUID, this, NewUUID);
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

	void Scene::Update(const float Delta)
	{
		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

		{
			for (auto& Script : *ScriptComponents)
			{
				Script.Instance->OnUpdate(Delta);
			}
		}

		// Sprite Rendering
		
		if (!ActiveCamera) return;

		auto& CameraComp = ActiveCamera->GetComponent<CameraComponent>();
		auto CameraPosition = ActiveCamera->GetComponent<TransformComponent>().GetGlobalPosition();
		
		glm::mat4 CameraProjection = CameraComp.GetProjection();
		glm::mat4 CameraTransform = CameraComp.GetTransform(CameraPosition);
		
		Renderer::Begin(CameraProjection, CameraTransform);

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
}