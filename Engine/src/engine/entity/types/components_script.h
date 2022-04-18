#pragma once

#include <engineincl.h>
#include <engine/entity/scriptable_entity.h>

#include "components_base.h"

namespace Techless
{
	struct ScriptComponent : public BaseComponent
	{
	public:
		// script component is entirely native so there's no way to serialise it without
		// extreme difficulty! therefore, i don't provide support for it at all :)

		Ptr<ScriptableEntity> Instance = nullptr;

		template<typename Script>
		Ptr<Script> Bind(Ptr<Entity> Owner)
		{
			Ptr<Script> newScript = CreatePtr<Script>();

			Instance = std::static_pointer_cast<ScriptableEntity>(newScript);
			Instance->LinkedEntity = Owner;
			Instance->OnCreate();

			return newScript;
		}

		template<typename Script>
		Ptr<Script> GetScript()
		{
			return std::static_pointer_cast<Script>(Instance);
		}

	private:
		bool Loaded = false;

		friend class Scene;

	};

	struct LuaScriptComponent : public BaseComponent
	{
	public:
		LuaScriptComponent() = default;
		LuaScriptComponent(const LuaScriptComponent& component) = default;

		void Unbind()
		{
			ScriptEnvironment::ResetEntity(LinkedEntity->GetScene()->GetLuaID(), LinkedEntity);
		}

		void Bind(std::string name)
		{
			if (!LinkedEntity)
				return;

			if (Loaded)
				Unbind();

			if (ScriptEnvironment::Has(name))
			{
				ScriptEnvironment::RegisterEntityScript(name, LinkedEntity);
				Name = name;

//				Debug::Log("Loaded script " + name + " into " + LinkedEntity->GetID(), "LuaScriptBinding");

				Loaded = true;
			}
			else
			{
//				Debug::Error("Script with name " + name + " does not exist.", "LuaScriptBinding");
			}
		}

		inline bool IsLoaded() const { return Loaded; };
		inline std::string GetScriptName() const { return Name; };

	private:
		std::string Name = "";
		bool Loaded = false;

		friend class Scene;

	public:

		inline friend void to_json(JSON& json, const LuaScriptComponent& component)
		{
			json = JSON{
				{"ScriptName", component.Name}
			};
		}

		inline friend void from_json(const JSON& json, LuaScriptComponent& component)
		{
			std::string name = json.at("ScriptName").get<std::string>();
			component.Name = name;
		}
	};
}