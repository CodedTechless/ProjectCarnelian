
#include "script_environment.h"

// Lua Initialisation

#include <lua.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


// Engine

#include <engine/lua/scriptable_lua_entity.hpp>

#include <engine/application/resource_loader.h>
#include <engine/application/watchdog/watchdog.h>

#include <engine/entity/components.h>
#include <engine/entity/prefabs/prefab_atlas.h>

#include <engine/application/application.h>
#include <engine/application/event.h>

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
	static std::vector<std::string> AccessibleLibraries = { 
		"print", "warn", "error", 
		"math", "coroutine", "string", 
		"require", "assert", "tostring", 
		"tonumber", "pcall", "inspect",
		"bool_tonumber", "sign"
	};

	std::unordered_map<std::string, LuaFunction> ScriptEnvironment::CachedScripts{};

	sol::state ScriptEnvironment::LuaVM{};

	namespace ScriptEnvironmentLogger
	{

		int LuaException(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description)
		{
			if (maybe_exception)
			{
				const std::exception& ex = *maybe_exception;
				Debug::Error(ex.what(), "Lua");
			}
			else
			{
				Debug::Error(description.data(), "Lua");
			}

			return sol::stack::push(L, description);
		}

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

	void ScriptEnvironment::End()
	{
		CachedScripts.clear();
	}

	size_t ScriptEnvironment::GetMemoryUsage()
	{
		return LuaVM.memory_used();
	}

	void ScriptEnvironment::Clean()
	{
		LuaVM.collect_garbage();
	}

	// sets up the global environment and all the special functions dedicated to it and stuff and shit like that innit
	void ScriptEnvironment::LoadGlobalEnvironment()
	{
		LuaVM.safe_script_file("assets/core_scripts/global_environment.lua", &sol::script_throw_on_error);
		sol::protected_function RegisterComponentType = LuaVM["RegisterComponentType"];

		// [COMPONENT ASSIGNMENT] LUA
		RegisterComponentType(TYPEID_STRING(TagComponent), "TagComponent");
		RegisterComponentType(TYPEID_STRING(TransformComponent), "TransformComponent");
		RegisterComponentType(TYPEID_STRING(RigidBodyComponent), "RigidBodyComponent");
		RegisterComponentType(TYPEID_STRING(SpriteComponent), "SpriteComponent");
		RegisterComponentType(TYPEID_STRING(SpriteAnimatorComponent), "SpriteAnimatorComponent");
		RegisterComponentType(TYPEID_STRING(CameraComponent), "CameraComponent");
		RegisterComponentType(TYPEID_STRING(LuaScriptComponent), "LuaScriptComponent");
		RegisterComponentType(TYPEID_STRING(ScriptComponent), "ScriptComponent");

		/*
		const std::vector<std::string> CoreScriptNames = {
			"RegisterScene", "DeregisterScene",
			"RegisterEntity", "DeregisterEntity",
			"RegisterComponent", "DeregisterComponent",
			"GetEntityBinding", "GetSceneBinding"
		};

		for (const std::string& Name : CoreScriptNames)
		{
			CachedCoreScripts[Name] = LuaVM[Name];
		}
		*/

		Debug::Log("Loaded global environment!", "ScriptEnvironment");
	}

	void ScriptEnvironment::Init()
	{
		Debug::Log("Initialising Lua...", "ScriptEnvironment");

		LuaVM.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::utf8);
		LuaVM.set_exception_handler(ScriptEnvironmentLogger::LuaException);

		LuaVM.script(R"(
			function ErrorHandler(string)
				cerror(string);
				return string;
			end
		)");

		sol::protected_function::set_default_handler(LuaVM["ErrorHandler"]);

		// to-do: make these binaries/combine them with source :)
		LuaVM.require_file("SceneBinding", "assets/core_scripts/scene_binding.lua", true, sol::load_mode::text);
		LuaVM.require_file("EntityBinding", "assets/core_scripts/entity_binding.lua", true, sol::load_mode::text);

		LuaVM.set_function("cprint", ScriptEnvironmentLogger::Log);
		LuaVM.set_function("cwarn", ScriptEnvironmentLogger::Warn);
		LuaVM.set_function("cerror", ScriptEnvironmentLogger::Error);

		{
			// #### Integral Types / Enums ####

			AccessibleLibraries.push_back("QueryMode");
			LuaVM.new_enum<QueryMode>("QueryMode", 
				{
					{ "Add", QueryMode::Add },
					{ "Remove", QueryMode::Remove }
				});

			AccessibleLibraries.push_back("InputFilter");
			LuaVM.new_enum<Input::Filter>("InputFilter",
				{
					{"Continue", Input::Filter::Continue},
					{"Ignore", Input::Filter::Stop},
					{"Stop", Input::Filter::Ignore}
				});

			AccessibleLibraries.push_back("InputState");
			LuaVM.new_enum<Input::State>("InputState",
				{
					{"None", Input::State::None},
					{"Begin", Input::State::Begin},
					{"Changed", Input::State::Changed},
					{"End", Input::State::End}
				});

			AccessibleLibraries.push_back("InputType");
			LuaVM.new_enum<Input::Type>("InputType",
				{
					{"None", Input::Type::None},
					{"Mouse", Input::Type::Mouse},
					{"Scrolling", Input::Type::Scrolling},
					{"Keyboard", Input::Type::Keyboard}
				});

			AccessibleLibraries.push_back("KeyCode");
			LuaVM.new_enum<Input::KeyCode>("KeyCode",
				{
					{"Keypad4", Input::KeyCode::Keypad4},
					{"KeypadAdd", Input::KeyCode::KeypadAdd},
					{"F16", Input::KeyCode::F16},
					{"Menu", Input::KeyCode::Menu},
					{"LeftAlt", Input::KeyCode::LeftAlt},
					{"Right", Input::KeyCode::Right},
					{"Keypad0", Input::KeyCode::Keypad0},
					{"KeypadEqual", Input::KeyCode::KeypadEqual},
					{"Backslash", Input::KeyCode::Backslash},
					{"LeftShift", Input::KeyCode::LeftShift},
					{"End", Input::KeyCode::End},
					{"Num8", Input::KeyCode::Num8},
					{"Num1", Input::KeyCode::Num1},
					{"F7", Input::KeyCode::F7},
					{"Tab", Input::KeyCode::Tab},
					{"RightSuper", Input::KeyCode::RightSuper},
					{"Minus", Input::KeyCode::Minus},
					{"F13", Input::KeyCode::F13},
					{"RightAlt", Input::KeyCode::RightAlt},
					{"RightControl", Input::KeyCode::RightControl},
					{"RightShift", Input::KeyCode::RightShift},
					{"F4", Input::KeyCode::F4},
					{"LeftControl", Input::KeyCode::LeftControl},
					{"F17", Input::KeyCode::F17},
					{"Keypad9", Input::KeyCode::Keypad9},
					{"Equal", Input::KeyCode::Equal},
					{"Backspace", Input::KeyCode::Backspace},
					{"NumLock", Input::KeyCode::NumLock},
					{"PrintScreen", Input::KeyCode::PrintScreen},
					{"F3", Input::KeyCode::F3},
					{"LeftBracket", Input::KeyCode::LeftBracket},
					{"KeypadDivide", Input::KeyCode::KeypadDivide},
					{"KeypadDecimal", Input::KeyCode::KeypadDecimal},
					{"KeypadEnter", Input::KeyCode::KeypadEnter},
					{"F11", Input::KeyCode::F11},
					{"F15", Input::KeyCode::F15},
					{"Semicolon", Input::KeyCode::Semicolon},
					{"F19", Input::KeyCode::F19},
					{"Enter", Input::KeyCode::Enter},
					{"Num2", Input::KeyCode::Num2},
					{"Keypad8", Input::KeyCode::Keypad8},
					{"Num0", Input::KeyCode::Num0},
					{"Num9", Input::KeyCode::Num9},
					{"Keypad7", Input::KeyCode::Keypad7},
					{"F22", Input::KeyCode::F22},
					{"Pause", Input::KeyCode::Pause},
					{"Escape", Input::KeyCode::Escape},
					{"Keypad5", Input::KeyCode::Keypad5},
					{"Keypad3", Input::KeyCode::Keypad3},
					{"Keypad2", Input::KeyCode::Keypad2},
					{"F23", Input::KeyCode::F23},
					{"Keypad1", Input::KeyCode::Keypad1},
					{"F25", Input::KeyCode::F25},
					{"F24", Input::KeyCode::F24},
					{"Keypad6", Input::KeyCode::Keypad6},
					{"Delete", Input::KeyCode::Delete},
					{"Period", Input::KeyCode::Period},
					{"X", Input::KeyCode::X},
					{"W", Input::KeyCode::W},
					{"Z", Input::KeyCode::Z},
					{"Y", Input::KeyCode::Y},
					{"T", Input::KeyCode::T},
					{"S", Input::KeyCode::S},
					{"V", Input::KeyCode::V},
					{"U", Input::KeyCode::U},
					{"F20", Input::KeyCode::F20},
					{"F18", Input::KeyCode::F18},
					{"F14", Input::KeyCode::F14},
					{"Apostrophe", Input::KeyCode::Apostrophe},
					{"PageDown", Input::KeyCode::PageDown},
					{"F12", Input::KeyCode::F12},
					{"F8", Input::KeyCode::F8},
					{"GraveAccent", Input::KeyCode::GraveAccent},
					{"H", Input::KeyCode::H},
					{"G", Input::KeyCode::G},
					{"J", Input::KeyCode::J},
					{"I", Input::KeyCode::I},
					{"D", Input::KeyCode::D},
					{"C", Input::KeyCode::C},
					{"F", Input::KeyCode::F},
					{"Comma", Input::KeyCode::Comma},
					{"P", Input::KeyCode::P},
					{"O", Input::KeyCode::O},
					{"None", Input::KeyCode::None},
					{"Q", Input::KeyCode::Q},
					{"L", Input::KeyCode::L},
					{"K", Input::KeyCode::K},
					{"N", Input::KeyCode::N},
					{"Num5", Input::KeyCode::Num5},
					{"Up", Input::KeyCode::Up},
					{"F9", Input::KeyCode::F9},
					{"F10", Input::KeyCode::F10},
					{"F6", Input::KeyCode::F6},
					{"F5", Input::KeyCode::F5},
					{"F2", Input::KeyCode::F2},
					{"LeftSuper", Input::KeyCode::LeftSuper},
					{"KeypadSubtract", Input::KeyCode::KeypadSubtract},
					{"F1", Input::KeyCode::F1},
					{"Num7", Input::KeyCode::Num7},
					{"B", Input::KeyCode::B},
					{"A", Input::KeyCode::A},
					{"RightBracket", Input::KeyCode::RightBracket},
					{"World2", Input::KeyCode::World2},
					{"Num6", Input::KeyCode::Num6},
					{"Left", Input::KeyCode::Left},
					{"E", Input::KeyCode::E},
					{"Home", Input::KeyCode::Home},
					{"CapsLock", Input::KeyCode::CapsLock},
					{"Down", Input::KeyCode::Down},
					{"Slash", Input::KeyCode::Slash},
					{"F21", Input::KeyCode::F21},
					{"Insert", Input::KeyCode::Insert},
					{"ScrollLock", Input::KeyCode::ScrollLock},
					{"World1", Input::KeyCode::World1},
					{"KeypadMultiply", Input::KeyCode::KeypadMultiply},
					{"R", Input::KeyCode::R},
					{"Space", Input::KeyCode::Space},
					{"M", Input::KeyCode::M},
					{"Num3", Input::KeyCode::Num3},
					{"PageUp", Input::KeyCode::PageUp},
					{"Num4", Input::KeyCode::Num4}
				});

			AccessibleLibraries.push_back("MouseCode");
			LuaVM.new_enum<Input::MouseCode>("MouseCode", 
				{
					{ "Button6", Input::MouseCode::Button6 },
					{ "Button3", Input::MouseCode::Button3 },
					{ "Button2", Input::MouseCode::Button2 },
					{ "Button0", Input::MouseCode::Button0 },
					{ "None", Input::MouseCode::None },
					{ "Button7", Input::MouseCode::Button7 },
					{ "Button5", Input::MouseCode::Button5 },
					{ "Button4", Input::MouseCode::Button4 },
					{ "Button1", Input::MouseCode::Button1 }
				});
		}

		{
			// #### Integral Types / Non-Constructable ####

			AccessibleLibraries.push_back("LightWindow");
			LuaVM.new_usertype<Window>("LightWindow",
					sol::no_constructor,

					"Size", sol::readonly(&Window::Size),
					"IsFocused", sol::readonly(&Window::IsFocused)
				);

			AccessibleLibraries.push_back("LightApplication");
			LuaVM.new_usertype<Application>("LightApplication",
					sol::no_constructor
				);

			AccessibleLibraries.push_back("InputEvent");
			LuaVM.new_usertype<InputEvent>("InputEvent", 
					sol::no_constructor,
					
					"InputType", sol::readonly(&InputEvent::InputType),
					"InputState", sol::readonly(&InputEvent::InputState),
					
					"KeyCode", sol::readonly(&InputEvent::KeyCode),
					"MouseCode", sol::readonly(&InputEvent::MouseCode),
					
					"Position", sol::readonly(&InputEvent::Position),
					"Delta", sol::readonly(&InputEvent::Delta)
				);

			AccessibleLibraries.push_back("WindowEvent");
			LuaVM.new_usertype<WindowEvent>("WindowEvent",
					sol::no_constructor,
					
					"Size", sol::readonly(&WindowEvent::Size),
					"Focused", sol::readonly(&WindowEvent::Focused)
				);

		}

		{
			// #### Integral Types / Constructable ####

			AccessibleLibraries.push_back("Vector2");
			LuaVM.new_usertype<Vector2>("Vector2",
					sol::constructors<Vector2(), Vector2(float, float)>(),

					"X", & Vector2::x,
					"Y", & Vector2::y,

					sol::meta_function::addition, sol::overload(
						[](Vector2 A, Vector2 B) -> Vector2 { return A + B; },
						[](Vector2 A, float B) -> Vector2 { return A + B; },
						[](float A, Vector2 B) -> Vector2 { return A + B; }
					),
					sol::meta_function::subtraction, sol::overload( 
						[](Vector2 A, Vector2 B) -> Vector2 { return A - B; },
						[](Vector2 A, float B) -> Vector2 { return A - B; },
						[](float A, Vector2 B) -> Vector2 { return A - B; }
					),
					sol::meta_function::multiplication, sol::overload(
						[](Vector2 A, Vector2 B) -> Vector2 { return A * B; },
						[](Vector2 A, float B) -> Vector2 { return A * B; },
						[](float A, Vector2 B) -> Vector2 { return A * B; }
					),
					sol::meta_function::division, sol::overload(
						[](Vector2 A, Vector2 B) -> Vector2 { return A / B; },
						[](Vector2 A, float B) -> Vector2 { return A / B; },
						[](float A, Vector2 B) -> Vector2 { return A / B; }
					),

					sol::meta_function::unary_minus, [](Vector2 A) -> Vector2 { return -A; }

				);

			AccessibleLibraries.push_back("Vector3");
			LuaVM.new_usertype<Vector3>("Vector3",
					sol::constructors<Vector3(), Vector3(float, float, float), Vector3(Vector2, float)>(),

					"X", &Vector3::x,
					"Y", &Vector3::y,
					"Z", &Vector3::z,

					sol::meta_function::addition, sol::overload(
						[](Vector3 A, Vector3 B) -> Vector3 { return A + B; },
						[](Vector3 A, float B) -> Vector3 { return A + B; },
						[](float A, Vector3 B) -> Vector3 { return A + B; }
					),
					sol::meta_function::subtraction, sol::overload(
						[](Vector3 A, Vector3 B) -> Vector3 { return A - B; },
						[](Vector3 A, float B) -> Vector3 { return A - B; },
						[](float A, Vector3 B) -> Vector3 { return A - B; }
					),
					sol::meta_function::multiplication, sol::overload(
						[](Vector3 A, Vector3 B) -> Vector3 { return A * B; },
						[](Vector3 A, float B) -> Vector3 { return A * B; },
						[](float A, Vector3 B) -> Vector3 { return A * B; }
					),
					sol::meta_function::division, sol::overload(
						[](Vector3 A, Vector3 B) -> Vector3 { return A / B; },
						[](Vector3 A, float B) -> Vector3 { return A / B; },
						[](float A, Vector3 B) -> Vector3 { return A / B; }
					),

					sol::meta_function::unary_minus, [](Vector3 A) -> Vector3 { return -A; }
				);

			AccessibleLibraries.push_back("Vector4");
			LuaVM.new_usertype<Vector4>("Vector4",
					sol::constructors<Colour(), Colour(float, float, float, float)>(),
					
					"R", &Colour::r,
					"G", &Colour::g,
					"B", &Colour::b,
					"A", &Colour::a
				);

			AccessibleLibraries.push_back("Prefab");
			LuaVM.new_usertype<Prefab>("Prefab",
					sol::no_constructor
				);

			AccessibleLibraries.push_back("Sprite");
			LuaVM.new_usertype<Sprite>("Sprite", 
					sol::no_constructor,

					"Name", sol::property(&Sprite::GetName),
					"Size", sol::property(&Sprite::GetSize)
				);

			AccessibleLibraries.push_back("SpriteAnimationSet");
			LuaVM.new_usertype<SpriteAnimationSet>("SpriteAnimationSet",
					sol::constructors<SpriteAnimationSet(std::string)>(),

					"Sequences", &SpriteAnimationSet::Sequences,
					"Default", &SpriteAnimationSet::Default,
					"Name", sol::readonly(&SpriteAnimationSet::Name)
				);

			AccessibleLibraries.push_back("SpriteAnimationSequence");
			LuaVM.new_usertype<SpriteAnimationSequence>("SpriteAnimationSequence",
					sol::constructors<SpriteAnimationSequence()>(),

					"Name", sol::readonly(&SpriteAnimationSequence::Name),
					"Add", &SpriteAnimationSequence::Add,

					"FrameRate", sol::property( &SpriteAnimationSequence::GetFrameRate, &SpriteAnimationSequence::SetFrameRate ),
					"Looped", &SpriteAnimationSequence::Looped
				);

			AccessibleLibraries.push_back("SpriteAnimationFrame");
			LuaVM.new_usertype<SpriteAnimationFrame>("SpriteAnimationFrame",
					sol::constructors<SpriteAnimationFrame()>(),
					
					"Sprite", &SpriteAnimationFrame::FrameSprite,
					"Length", &SpriteAnimationFrame::Length
				);
		}

		{
			// #### Static ####

			AccessibleLibraries.push_back("Input");
			LuaVM.new_usertype<Input>("Input",
					sol::no_constructor,

					"KeyDown", &Input::KeyDown,
					"MouseButtonDown", &Input::MouseButtonDown,
					"GetMousePosition", &Input::GetMousePosition
				);

			AccessibleLibraries.push_back("PrefabAtlas");
			LuaVM.new_usertype<PrefabAtlas>("PrefabAtlas",
					sol::no_constructor,
					"Get", &PrefabAtlas::Get
				);

			AccessibleLibraries.push_back("SpriteAtlas");
			LuaVM.new_usertype<SpriteAtlas>("SpriteAtlas", 
					sol::no_constructor,
					"Get", &SpriteAtlas::Get
				);

			AccessibleLibraries.push_back("AnimationAtlas");
			LuaVM.new_usertype<AnimationAtlas>("AnimationAtlas",
					sol::no_constructor,
					
					"Get", &AnimationAtlas::Get
				);
		}

		{
			// #### Light Types ####


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
					"CreateEntity", &Scene::CreateEntity,
					"Instantiate", &Scene::Instantiate,
					
					"GetActiveCamera", &Scene::GetActiveCamera,
					"SetActiveCamera", &Scene::SetActiveCamera,
					
					"ScriptRuntimeEnabled", sol::property( &Scene::IsScriptExecutionEnabled , &Scene::SetScriptExecutionEnabled )
				);
		}

		{
			// #### Component Types ####

			// [COMPONENT ASSIGNMENT] Lua

			AccessibleLibraries.push_back("TagComponent");
			LuaVM.new_usertype<TagComponent>("TagComponent",
				sol::no_constructor,
				"Name", &TagComponent::Name
			);

			AccessibleLibraries.push_back("TransformComponent");
			LuaVM.new_usertype<TransformComponent>("TransformComponent",
				sol::no_constructor,
				"Position", sol::property(&TransformComponent::GetLocalPosition, &TransformComponent::SetLocalPosition),
				"Scale", sol::property(&TransformComponent::GetLocalScale, &TransformComponent::SetLocalScale),
				"Orientation", sol::property(&TransformComponent::GetLocalOrientation, &TransformComponent::SetLocalOrientation),

				"GlobalPosition", sol::property(&TransformComponent::GetGlobalPosition),
				"GlobalScale", sol::property(&TransformComponent::GetGlobalScale),
				"GlobalOrientation", sol::property(&TransformComponent::GetGlobalOrientation),

				"SetEngineInterpolationEnabled", &TransformComponent::SetEngineInterpolation
			);

			AccessibleLibraries.push_back("RigidBodyComponent");
			LuaVM.new_usertype<RigidBodyComponent>("RigidBodyComponent",
				sol::no_constructor,
				"Velocity", &RigidBodyComponent::Velocity,
				"GroundFriction", &RigidBodyComponent::GroundFriction,
				"AirFriction", &RigidBodyComponent::AirFriction
			);

			AccessibleLibraries.push_back("SpriteComponent");
			LuaVM.new_usertype<SpriteComponent>("SpriteComponent",
				sol::no_constructor,
				"TintColour", &SpriteComponent::SpriteColour,
				"SetSprite", &SpriteComponent::SetSprite,
				"GetSprite", &SpriteComponent::GetSprite
			);
			
			AccessibleLibraries.push_back("SpriteAnimatorComponent");
			LuaVM.new_usertype<SpriteAnimatorComponent>("SpriteAnimatorComponent",
				sol::no_constructor,

				"Frame", &SpriteAnimatorComponent::Frame,
				"Paused", &SpriteAnimatorComponent::Paused,
				"Play", &SpriteAnimatorComponent::Play,
				
				"AnimationSet", sol::property( &SpriteAnimatorComponent::GetAnimationSet , &SpriteAnimatorComponent::SetAnimationSet ),
				"CurrentAnimation", sol::property( &SpriteAnimatorComponent::GetCurrentAnimation )
			);

			AccessibleLibraries.push_back("CameraComponent");
			LuaVM.new_usertype<CameraComponent>("CameraComponent",
				sol::no_constructor,
				"SetProjection", &CameraComponent::SetProjection,
				"GetViewportResolution", &CameraComponent::GetViewportResolution
			);

			AccessibleLibraries.push_back("LuaScriptComponent");
			LuaVM.new_usertype<LuaScriptComponent>("LuaScriptComponent",
				sol::no_constructor,
				"Bind", &LuaScriptComponent::Bind
			);

			AccessibleLibraries.push_back("ScriptComponent");
			LuaVM.new_usertype<ScriptComponent>("ScriptComponent", 
				sol::no_constructor
			);
		}

		LoadGlobalEnvironment();
		
		ResourceLoader::GetFiles(Resource::LuaScript,
			[](const fs::directory_entry& File)
			{
				fs::path Path = File.path();
				std::string sPath = Path.generic_string();

				std::string Name = Path.stem().string();
				sol::load_result LoadResult = LuaVM.load_file(sPath);

				CachedScripts[Name] = LoadResult;

				if (!LoadResult.valid())
				{
					sol::error err = LoadResult;
					Debug::Error("Error while loading " + sPath + ": " + err.what(), "ScriptEnvironment");

					return;
				}

				Debug::Log("Loaded " + sPath + " to " + Name, "ScriptEnvironment");
			});

		AccessibleLibraries.push_back("Application");
		LuaVM["Application"] = Application::GetActiveApplication();
		AccessibleLibraries.push_back("Window");
		LuaVM["Window"] = Application::GetActiveApplication().GetActiveWindow();

		Debug::Log("Initialised Lua!", "ScriptEnvironment");
	}

	bool ScriptEnvironment::Has(const std::string& Name)
	{
		return CachedScripts.find(Name) != CachedScripts.end();
	}

	// generic scripts return a LuaEnv to play around with because they might need to be accessed from C++
	UPtr<LuaEnv> ScriptEnvironment::RegisterGenericScript(const std::string& ScriptName)
	{
		UPtr<LuaEnv> NewEnvironment = CreateUPtr<LuaEnv>( LuaVM, sol::create );

		for (const std::string& entry : AccessibleLibraries)
			NewEnvironment->set(entry, LuaVM[entry]);

		sol::protected_function& FuncRef = CachedScripts[ScriptName];

		NewEnvironment->set_on(FuncRef);
		CachedScripts[ScriptName].call(FuncRef);

		return NewEnvironment;
	}

	// as entities scripts are managed by lua, nothing has to be returned (as there's no need to call an entities scripts from C++)
	void ScriptEnvironment::RegisterEntityScript(const std::string& ScriptName, Entity* entity)
	{
		sol::table EntityTable = GetEntityBinding(entity->GetScene()->GetLuaID(), entity->GetID());

		// Build environment
		LuaEnv NewEnvironment = { LuaVM, sol::create, EntityTable };

		for (const auto& entry : AccessibleLibraries)
			NewEnvironment[entry] = LuaVM[entry];
		
		sol::metatable env_metatable = NewEnvironment[sol::metatable_key];
		env_metatable["__index"] = EntityTable;
		env_metatable["__newindex"] = [EntityTable](sol::table t, sol::object k, sol::object v) {
			sol::table e = EntityTable;
			e[k] = v;
		};

		// Run template function
		sol::protected_function& FuncRef = CachedScripts[ScriptName];

		NewEnvironment.set_on(FuncRef);
		sol::protected_function_result res = FuncRef.call();

		if (!res.valid())
		{
			sol::error err = res;
			Debug::Error("Error while initialising entity script " + ScriptName + ": " + err.what(), "ScriptEnvironment");
		}
		else
		{
			if (entity->GetScene()->IsScriptExecutionEnabled())
				NewEnvironment.get<sol::protected_function>("OnCreated")();
		}

		//LuaVM["inspect"](env_metatable);
		//LuaVM["inspect"](Environment);
		//LuaVM["inspect"](EntityTable);
	}

	void ScriptEnvironment::RegisterEntity(int EnvironmentID, Entity* entity) 
	{ 
		LuaVM.get<sol::protected_function>("RegisterEntity")(EnvironmentID, entity); 
	};

	int ScriptEnvironment::RegisterScene(Ptr<Scene> scene)
	{ 
		return LuaVM.get<sol::protected_function>("RegisterScene")(scene); 
	};

	void ScriptEnvironment::ResetEntity(int SceneID, Entity* entity)
	{
		LuaVM.get<sol::protected_function>("ResetEntity")(SceneID, entity);
	}

}