#pragma once

#include "entity.h"

namespace Techless 
{

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {};

		template<typename Component>
		Component& AddComponent()
		{
			return LinkedEntity->AddComponent<Component>();
		}

		template<typename Component>
		Component& GetComponent()
		{
			return LinkedEntity->GetComponent<Component>();
		}

		template<typename Component>
		void RemoveComponent()
		{
			LinkedEntity->RemoveComponent<Component>();
		}

		template<typename Component>
		bool HasComponent()
		{
			return LinkedEntity->HasComponent<Component>();
		}

	protected:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};

		virtual void OnFixedUpdate(const float Delta) {};
		virtual void OnUpdate(const float Delta) {};
		virtual void OnDraw(const float Delta) {};

	private:
		Entity* LinkedEntity;

		friend struct ScriptComponent;
		friend class Scene;
	};

}