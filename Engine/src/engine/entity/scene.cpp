

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

	Ptr<Entity> Scene::GetEntityByID(const std::string& EntityID)
	{
		for (Ptr<Entity> entity : Entities)
		{
			if (entity->GetID() == EntityID)
			{
				return entity;
			}
		}
	}

	Ptr<Entity> Scene::CreateBlankEntity()
	{
		std::string NewUUID = UUID::Generate();

		Ptr<Entity> Ent = CreatePtr<Entity>(this, NewUUID);
		ScriptEnvironment::RegisterEntity(SceneLuaID, Ent);

		return Ent;
	}

	Ptr<Entity> Scene::CreateEntity(const std::string& TagName)
	{
		std::string NewUUID = UUID::Generate();

		Ptr<Entity> Ent = CreatePtr<Entity>(this, NewUUID);
		ScriptEnvironment::RegisterEntity(SceneLuaID, Ent);

		Ent->AddComponent<TransformComponent>();

		auto& Tag = Ent->AddComponent<TagComponent>();
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

	template <typename ComponentType>
	static void DuplicateComponent(Entity& SourceEntity, Entity& DestEntity)
	{
		if (SourceEntity.HasComponent<ComponentType>())
			DestEntity.AddComponent<ComponentType>(SourceEntity.GetComponent<ComponentType>());
	}

	/*Entity& Scene::DuplicateEntity(Entity& dupeEntity, Entity& parent)
	{

	}*/

	template <typename ComponentType>
	static void AssignComponents(Scene& scene, std::unordered_map<uint16_t, Ptr<Entity>>& CreatedEntities, Prefab& prefab)
	{
		auto Components = prefab.GetComponents<ComponentType>();

		for (auto& [ID, Component] : *Components)
		{
			Ptr<Entity> entity = nullptr;

			if (CreatedEntities.find(ID) == CreatedEntities.end())
			{
				entity = scene.CreateBlankEntity();
				CreatedEntities[ID] = entity;
			}
			else
				entity = CreatedEntities[ID];

			entity->AddComponent<ComponentType>(Component);
		}
	}

	Ptr<Entity> Scene::Instantiate(Prefab& prefab)
	{
		std::unordered_map<uint16_t, Ptr<Entity>> Entities = {};

		// [COMPONENT ASSIGNMENT]
		// Assign all of the relevant components!
		AssignComponents<TagComponent>				(*this, Entities, prefab);
		AssignComponents<TransformComponent>		(*this, Entities, prefab);
		AssignComponents<BoxColliderComponent>		(*this, Entities, prefab);
		AssignComponents<YSortComponent>			(*this, Entities, prefab);
//		AssignComponents<RigidBodyComponent>		(*this, Entities, prefab);

		AssignComponents<SpriteComponent>			(*this, Entities, prefab);
		AssignComponents<SpriteAnimatorComponent>	(*this, Entities, prefab);
		AssignComponents<CameraComponent>			(*this, Entities, prefab);
		
		AssignComponents<LuaScriptComponent>		(*this, Entities, prefab);

		for (int i = 0; i < prefab.ParentalIndex.size(); ++i)
		{
			int ParentalIndex = prefab.ParentalIndex[i];

			if (ParentalIndex != -1)
				Entities[i]->SetParent(Entities[ParentalIndex]);
		}

		{
			for (auto& [ID, entity] : Entities)
			{
				if (entity->HasComponent<LuaScriptComponent>())
				{
					auto& Script = entity->GetComponent<LuaScriptComponent>();
					
					if (Script.IsLoaded() == false && Script.GetScriptName() != "")
					{
						Script.Bind(Script.GetScriptName());
					}
				}
			}
		}

		// there should ALWAYS be an Entities[0]. it should always be the root entity.
		// if it's not, then it's undefined behaviour!!!!!!!!
		return Entities[0];
	}

	Input::Filter Scene::OnInputEvent(InputEvent inputEvent, bool Processed)
	{
		if (inputEvent.InputType == Input::Type::Mouse)
		{
			auto& c_Camera = ActiveCamera->GetComponent<CameraComponent>();

			inputEvent.Position = c_Camera.ScreenToViewportCoordinates(inputEvent.Position);

			Viewport viewport = c_Camera.GetViewport();
			if (inputEvent.Position.x > viewport.Size.x || inputEvent.Position.y > viewport.Size.y || inputEvent.Position.x < 0 || inputEvent.Position.y < 0)
			{
				return Input::Filter::Ignore;
			}
		}

		/*
			to-do: (list)
			 - make it so inputs propagate from positive to negative depths
			 - do it efficiently (don't sort on z-depth every input because that's really fucking inefficient bitch)
		*/
		if (FLAG_ScriptExecutionEnabled)
		{
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

				if (LuaFilter == Input::Filter::Stop)
					FinalFilter = Input::Filter::Stop;
				else if (LuaFilter == Input::Filter::Continue)
					Processed = true;
			}

			return FinalFilter;
		}

		return Input::Filter::Ignore;
	}

	void Scene::OnWindowEvent(const WindowEvent& windowEvent)
	{
		if (ActiveCamera)
		{
			auto& c_Camera = ActiveCamera->GetComponent<CameraComponent>();

			if (c_Camera.AutoViewportResizeToWindow)
				c_Camera.SetViewportSize(windowEvent.Size);
		}

		if (FLAG_ScriptExecutionEnabled)
		{
			auto ScriptComponents = SceneRegistry.GetRegistrySet<ScriptComponent>();

			for (auto& Script : *ScriptComponents)
			{
				Script.Instance->OnWindowEvent(windowEvent);
			}

			ScriptEnvironment::CallScene(SceneLuaID, "OnWindowEvent", windowEvent);
		}
	}

	void Scene::FixedUpdate(float Delta)
	{
		SceneRegistry.View<TransformComponent>(
			[](TransformComponent& Transform)
			{
				Transform.ForceInterpolationUpdate();
			});

		if (FLAG_ScriptExecutionEnabled)
		{


			SceneRegistry.View<ScriptComponent>(
				[&](ScriptComponent& Script)
				{
					Script.Instance->OnFixedUpdate(Delta);
				});

			ScriptEnvironment::CallScene(SceneLuaID, "OnFixedUpdate", Delta);
		}
	}


	struct SceneRenderInfo
	{
		SpriteComponent* SpritePtr = nullptr;
		TransformComponent* TransformPtr = nullptr;
	};

	void Scene::Update(float Delta)
	{
		// Sprite Rendering

		SceneRegistry.View<TransformComponent>(
			[](TransformComponent& Transform)
			{
				Transform.MarkInterpolationDirty();
			});
		
		SceneRegistry.View<SpriteAnimatorComponent>(
			[&](SpriteAnimatorComponent& c_SpriteAnimator)
			{
				c_SpriteAnimator.Update(Delta);
			});

		if (!ActiveCamera) return;

		CameraComponent& cam_Camera = ActiveCamera->GetComponent<CameraComponent>();
		TransformComponent& cam_Transform = ActiveCamera->GetComponent<TransformComponent>();
		
		if (cam_Camera.IsFramebufferMode())
		{
			cam_Camera.m_FrameBuffer->Bind();
			Renderer::ResetClearColour();
			Renderer::Clear();
		}
		else
		{
			Renderer::SetViewport(cam_Camera.GetViewport());
		}

		Mat4x4 projection = cam_Camera.GetProjection();
		
		{
			Renderer::Begin(projection, cam_Camera.GetTransform(cam_Transform.GetGlobalPosition()));

			if (FLAG_ScriptExecutionEnabled)
			{
				SceneRegistry.View<ScriptComponent>(
					[&](ScriptComponent& Script)
					{
						Script.Instance->OnUpdate(Delta);
					});

				ScriptEnvironment::CallScene(SceneLuaID, "OnUpdate", Delta);
			}

			{
				std::vector<SceneRenderInfo> SceneSprites{};

				SceneRegistry.View<SpriteComponent, TransformComponent>(
					[&](SpriteComponent& c_Sprite, TransformComponent& c_Transform)
					{
						if (c_Sprite.Visible == false || c_Sprite.SpriteColour.a == 0.f)
							return; // don't bother rendering it if it's not visible



						Ptr<Sprite> aSprite = c_Sprite.GetSprite();
						if (aSprite)
							SceneSprites.push_back({ &c_Sprite, &c_Transform });

					});

				std::sort(SceneSprites.begin(), SceneSprites.end(), [](SceneRenderInfo& a, SceneRenderInfo& b)
					{
						return a.TransformPtr->GetGlobalPosition().z < b.TransformPtr->GetGlobalPosition().z;
					});

				for (SceneRenderInfo& renderInfo : SceneSprites)
				{
					Renderer::DrawSpriteExt(renderInfo.SpritePtr->GetSprite(), renderInfo.TransformPtr->GetGlobalTransform(), renderInfo.SpritePtr->SpriteColour);
				}
			}

			/*
			SceneRegistry.View<TransformComponent>(
				[&](TransformComponent& c_Transform)
				{
					Renderer::DrawQuad({ glm::vec2(c_Transform.GetGlobalTransformState().Position), 80.f }, { 1.f, 1.f }, 0.f, { 0.f, 1.f, 0.f, 0.5f });
					Renderer::DrawQuad({ glm::vec2(c_Transform.GetPreviousState().Position), 80.f }, { 1.f, 1.f }, 0.f, { 0.f, 0.f, 1.f, 0.5f });
					Renderer::DrawQuad({ glm::vec2(c_Transform.GetCurrentState().Position), 80.f }, { 1.f, 1.f }, 0.f, { 1.f, 0.f, 0.f, 0.5f });
				});
			*/

			Renderer::End();
		}

		{
			Renderer::Begin(projection, Mat4x4(1.f));

			SceneRegistry.View<UITransformComponent>(
				[](UITransformComponent& UITransform)
				{

				});

			Renderer::End();
		}
		
		if (cam_Camera.IsFramebufferMode())
		{
			cam_Camera.m_FrameBuffer->Unbind();
		}
	}

	void Scene::Serialise(const std::string& FilePath, Ptr<Entity> RootEntity)
	{
		Serialiser l_Serialiser = { RootEntity };
		l_Serialiser.SaveToFile(FilePath);
	}
}