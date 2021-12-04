#pragma once

#include <engine/entity/registry/registry.h>
#include <engine/application/event.h>

namespace Techless 
{
	class Entity;

	class Scene
	{
	public:
		Entity& CreateEntity();
		void DestroyEntity(const std::string& EntityID);

		void Update(const float Delta);
		void FixedUpdate(const float Delta);

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
		Registry SceneRegistry;
		
	private:
		Entity* ActiveCamera;

		friend class Entity;
	};
}