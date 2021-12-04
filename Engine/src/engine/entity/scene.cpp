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
		assert(ActiveCamera != nullptr);

		auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();
		for (auto Script : *ScriptComponents)
		{
			Script.Instance->OnUpdate(Delta);
		}

		auto CamProjection = ActiveCamera->GetComponent<CameraComponent>().GetProjection();
		auto CamPosition = ActiveCamera->GetComponent<TransformComponent>().Position;
		
		glm::mat4 Transform = glm::translate(glm::mat4(1.f), glm::vec3(CamPosition, 0));
		
		Renderer::Begin(CamProjection, Transform);

		for (auto Script : *ScriptComponents)
		{
			Script.Instance->OnDraw(Delta);
		}

		auto SpriteComponents = SceneRegistry.GetRegistrySet<SpriteComponent>();
		auto TransformComponents = SceneRegistry.GetRegistrySet<TransformComponent>();

		unsigned int i = 0;
		for (auto Sprite : *SpriteComponents)
		{
			auto EntityID = SpriteComponents->GetIDAtIndex(i);
			auto& Transform = TransformComponents->Get(EntityID);

			Renderer::DrawSprite(Sprite.aSprite, Transform.Position + glm::vec2(Sprite.Offset.x, Sprite.Offset.y), Transform.Scale, Transform.Depth + Sprite.Offset.z, Transform.Angle, Sprite.Colour, Sprite.Alpha);

			++i;
		}

		Renderer::End();
	}
}