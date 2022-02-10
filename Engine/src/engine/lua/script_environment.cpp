#include "script_environment.h"

#include <engine/maths/vector.hpp>
#include <engine/watchdog/watchdog.h>

#include <engine/entity/component/components.h>
#include <engine/entity/scriptable_entity.h>

#include <engine/lua/identities/scriptable_lua_entity.hpp>

#include <engineincl.h>

namespace fs = std::filesystem;

/*

lua does:
 - request create/destroy entity
 - request add/remove components
 - caches pointers to entities/components on its own end so it can alter their properties directly w/ sol

c++ does:
 - pushing entity/component pointers to lua when new entities/components get created
 - telling lua when an entity or a component gets removed so lua can uncache them

*/

namespace Techless
{
	static const std::string AccessibleLibraries[8] = { "print", "warn", "error", "math", "coroutine", "string", "require", "assert" };

	std::unordered_map<std::string, sol::protected_function> ScriptEnvironment::CachedScripts{};
	sol::state ScriptEnvironment::LuaVM;

	namespace ScriptEnvironmentLogger
	{

		void ScriptError(sol::protected_function_result res)
		{
			sol::error err = res;

			Debug::Error(err.what(), "Lua");
		}

		void Log(std::string String)
		{
			Debug::Log(String, "Lua");
		}

		void Warn(std::string String)
		{
			Debug::Warn(String, "Lua");
		}

		void Error(std::string String)
		{
			Debug::Error(String, "Lua");
		}
	}

	void ScriptEnvironment::Read(const std::string& Path)
	{
		for (const auto& File : fs::directory_iterator(Path))
		{
			auto FsPath = File.path();
			auto Path = FsPath.generic_string();

			if (File.is_directory())
			{
				Read(Path);
			}
			else if (File.is_regular_file() && Path.substr(Path.size() - 4) == ".lua")
			{
				std::string Name = FsPath.stem().string();
				CachedScripts[Name] = LuaVM.load_file(Path);

				Debug::Log("Loaded " + Path + " to " + Name, "ScriptAtlas");
			}
		}
	}


	void ScriptEnvironment::Init()
	{
		LuaVM.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math);

		// to-do: make these binaries/combine them with source :)
		LuaVM.require_file("SceneBinding", "assets/scripts/core/scene_binding.lua", true, sol::load_mode::text);
		LuaVM.require_file("EntityBinding", "assets/scripts/core/entity_binding.lua", true, sol::load_mode::text);

		LuaVM.safe_script_file("assets/scripts/core/environment.lua", &ScriptEnvironmentLogger::ScriptError);

		RegisterScene = { LuaVM["RegisterScene"] , &ScriptEnvironmentLogger::ScriptError };
		DeregisterScene = { LuaVM["DeregisterScene"] , &ScriptEnvironmentLogger::ScriptError };
		
		RegisterEntity = { LuaVM["RegisterEntity"] , &ScriptEnvironmentLogger::ScriptError };
		DeregisterEntity = { LuaVM["DeregisterEntity"] , &ScriptEnvironmentLogger::ScriptError };

		RegisterComponent = { LuaVM["RegisterComponent"] , &ScriptEnvironmentLogger::ScriptError };
		DeregisterComponent = { LuaVM["DeregisterComponent"] , &ScriptEnvironmentLogger::ScriptError };

		GetEntityBinding = { LuaVM["GetEntityBinding"] , &ScriptEnvironmentLogger::ScriptError };

		sol::protected_function RegisterComponentType = LuaVM["RegisterComponentType"];

		// [COMPONENT ASSIGNMENT]

		RegisterComponentType(TYPEID_STRING(TagComponent), "TagComponent");
		RegisterComponentType(TYPEID_STRING(TransformComponent), "TransformComponent");
		RegisterComponentType(TYPEID_STRING(RigidBodyComponent), "RigidBodyComponent");
		RegisterComponentType(TYPEID_STRING(SpriteComponent), "SpriteComponent");
		RegisterComponentType(TYPEID_STRING(CameraComponent), "CameraComponent");

		LuaVM.set_function("print", ScriptEnvironmentLogger::Log);
		LuaVM.set_function("warn", ScriptEnvironmentLogger::Warn);
		LuaVM.set_function("error", ScriptEnvironmentLogger::Error);

		LuaVM.new_enum<QueryMode>("QueryMode", {
			{ "Add", QueryMode::Add },
			{ "Remove", QueryMode::Remove }
		});
		
		LuaVM.new_usertype<glm::vec2>("Vector2",
			sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
			"X", &glm::vec2::x,
			"Y", &glm::vec2::y
		);

		LuaVM.new_usertype<glm::vec3>("Vector3",
			sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
			"X", &glm::vec3::x,
			"Y", &glm::vec3::y,
			"Z", &glm::vec3::z
		);

		LuaVM.new_usertype<Colour>("Colour",
			sol::constructors<Colour(), Colour(float, float, float), Colour(float, float, float, float), Colour(int, int, int), Colour(int, int, int, int)>(),
			"R", &Colour::R,
			"G", &Colour::G,
			"B", &Colour::B,
			"A", &Colour::A,
			"SetRGBColour", &Colour::SetRGBColour
		);

		LuaVM.new_usertype<LuaScriptableEntity>("LuaScriptableEntity",
			sol::constructors<LuaScriptableEntity(Entity*)>(),

			"ID", sol::property(&LuaScriptableEntity::GetID),
			"Parent", sol::property(&LuaScriptableEntity::GetParent, &LuaScriptableEntity::SetParent),
			"GetChildren", &LuaScriptableEntity::GetChildren,

			"GetLightScene", &LuaScriptableEntity::GetLinkedScene,
			"GetLightEntity", &LuaScriptableEntity::GetLinkedEntity,

			"QueryComponent", &LuaScriptableEntity::QueryComponent
		);

		LuaVM.new_usertype<Entity>("LightEntity",
			sol::no_constructor,
			"ID", sol::property(&Entity::GetID),
			"GetLightScene", &Entity::GetScene
		);

		LuaVM.new_usertype<Scene>("LightScene",
			sol::no_constructor,
			"CreateEntity", &Scene::CreateEntity
		);

		{
			// [COMPONENT ASSIGNMENT]

			LuaVM.new_usertype<TagComponent>("TagComponent",
				sol::no_constructor,
				"Name", &TagComponent::Name
			);

			LuaVM.new_usertype<TransformComponent>("TransformComponent",
				sol::no_constructor,
				"LocalPosition", sol::property( &TransformComponent::GetLocalPosition, &TransformComponent::SetLocalPosition ),
				"LocalScale", sol::property( &TransformComponent::GetLocalScale, &TransformComponent::SetLocalScale ),
				"LocalOrientation", sol::property( &TransformComponent::GetLocalOrientation, &TransformComponent::SetLocalOrientation ),
				
				"GlobalPosition", sol::property( &TransformComponent::GetGlobalPosition ),
				"GlobalScale", sol::property( &TransformComponent::GetGlobalScale ),
				"GlobalOrientation", sol::property( &TransformComponent::GetGlobalOrientation )
			);

			LuaVM.new_usertype<RigidBodyComponent>("RigidBodyComponent",
				sol::no_constructor,
				"Velocity", &RigidBodyComponent::Velocity,
				"Friction", &RigidBodyComponent::Friction
			);

			LuaVM.new_usertype<SpriteComponent>("SpriteComponent",
				sol::no_constructor,
				"TintColour", &SpriteComponent::SpriteColour,
				"Sprite", sol::property( &SpriteComponent::GetSpriteName, &SpriteComponent::SetSprite )
			);

			LuaVM.new_usertype<CameraComponent>("CameraComponent",
				sol::no_constructor,
				"SetProjection", &CameraComponent::SetProjection,
				"GetViewportResolution", &CameraComponent::GetViewportResolution
			);
		}
	}

	Ptr<sol::environment> ScriptEnvironment::Create(const std::string& Name, Entity* entity)
	{
		sol::object EntityTable = GetEntityBinding(entity->GetScene(), entity->GetID());

		Ptr<sol::environment> NewEnvironment = CreatePtr<sol::environment>(LuaVM, sol::create, EntityTable);
		sol::environment& Environment = *NewEnvironment;

		for (const auto& entry : AccessibleLibraries)
			Environment[entry] = LuaVM[entry];

		sol::set_environment(Environment, CachedScripts[Name]);
		CachedScripts[Name].call();

		return NewEnvironment;
	}

}