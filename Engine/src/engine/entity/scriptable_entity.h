#pragma once

#include "entity.h"

#include <engine/application/input/input.h>
#include <engine/application/event.h>

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

		inline Scene* GetScene() const
		{
			return LinkedEntity->GetScene();
		}

		inline Entity* GetSelf() const 
		{
			return LinkedEntity;
		}

	protected:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};

		virtual void OnFixedUpdate(const float Delta) {};
		virtual void OnUpdate(const float Delta) {};
		virtual void OnDraw(const float Delta) {};

		virtual Input::Filter OnInputEvent(const InputEvent& inputEvent, const bool Processed) { return Input::Filter::Ignore; };
		virtual void OnWindowEvent(const WindowEvent& inputEvent) {};

	private:
		Entity* LinkedEntity = nullptr;

		friend struct ScriptComponent;
		friend class Scene;
	};

}