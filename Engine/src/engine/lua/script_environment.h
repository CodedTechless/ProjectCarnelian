#pragma once

#include <lua.hpp>

#include <sol/sol.hpp>
#include <engineincl.h>

namespace Techless
{
	using ScriptEnv = sol::environment;
	using Script = sol::protected_function;

	class Entity;
	class Scene;

	class ScriptEnvironment
	{
	public:

		static void Init();
		
		static Ptr<ScriptEnv> Create(const std::string& Name, Entity* entity);
		static Ptr<ScriptEnv> CreateGlobal(const std::string& Name);
		static bool Has(const std::string& Name);

		static int RegisterScene(Ptr<Scene> scene);
		static void DeregisterScene(int EnvironmentID) { CachedCoreScripts["DeregisterScene"](EnvironmentID); };

		static void RegisterEntity(int EnvironmentID, Entity* entity);
		static void DeregisterEntity(int EnvironmentID, const std::string& entityID) { CachedCoreScripts["DeregisterEntity"](EnvironmentID, entityID); };

		template<typename T>
		static void RegisterComponent(int EnvironmentID, const std::string& EntityID, T* LinkedComponent) 
		{
			CachedCoreScripts["RegisterComponent"](EnvironmentID, EntityID, TYPEID_STRING(T), LinkedComponent);
		};

		template<typename T>
		static void DeregisterComponent(int EnvironmentID, const std::string& EntityID) 
		{
			CachedCoreScripts["DeregisterComponent"](EnvironmentID, EntityID, TYPEID_STRING(T));
		};

		static sol::table GetEntityBinding(int EnvironmentID, const std::string& EntityID)
		{
			return CachedCoreScripts["GetEntityBinding"](EnvironmentID, EntityID);
		};

		static sol::table GetSceneBinding(int EnvironmentID)
		{
			return CachedCoreScripts["GetSceneBinding"](EnvironmentID);
		}

	private:
		static std::unordered_map<std::string, Script> CachedCoreScripts;
		static std::unordered_map<std::string, Script> CachedScripts;
		
		static void Read(const std::string& Path);
		static void LoadCoreScripts();

		static sol::state LuaVM;
	};

}