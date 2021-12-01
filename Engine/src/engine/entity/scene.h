#pragma once

#include <engine/entity/registry/registry.h>

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
		void Draw();

		inline void SetActiveCamera(Entity& entity) { ActiveCamera = &entity; };
		inline Entity& GetActiveCamera() const { return *ActiveCamera; };

		template<typename Type>
		Ptr<TypedRegistrySet<Type>> GetInstances()
		{
			return SceneRegistry.GetRegistrySet<Type>();
		}

	private:
		Registry SceneRegistry;
		
	private:
		Entity* ActiveCamera;

		friend class Entity;
	};
}