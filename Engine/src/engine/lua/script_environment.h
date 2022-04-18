#pragma once

#include <lua.hpp>

#include <sol/sol.hpp>
#include <engineincl.h>

namespace Techless
{
	using LuaEnv = sol::environment;
	using LuaFunction = sol::protected_function;

	class Entity;
	class Scene;

	class ScriptEnvironment
	{
	public:
		static void Init();
		static void End();

		static void Clean();
		static size_t GetMemoryUsage();

	public:
		static void RegisterEntityScript(const std::string& ScriptName, Ptr<Entity> entity);
		static UPtr<LuaEnv> RegisterGenericScript(const std::string& ScriptName);

		static bool Has(const std::string& Name);

		static int RegisterScene(Ptr<Scene> scene);
		static void DeregisterScene(int EnvironmentID) { LuaVM.get<sol::protected_function>("DeregisterScene")(EnvironmentID); };

		static void RegisterEntity(int EnvironmentID, Ptr<Entity> entity);
		static void DeregisterEntity(int EnvironmentID, const std::string& entityID) { LuaVM.get<sol::protected_function>("DeregisterEntity")(EnvironmentID, entityID); };
		
		static void RegisterChild(int EnvironmentID, const std::string& ParentID, const std::string& ChildID) { LuaVM.get<sol::protected_function>("RegisterChild")(EnvironmentID, ParentID, ChildID); };
		static void DeregisterChild(int EnvironmentID, const std::string& ParentID, const std::string& ChildID) { LuaVM.get<sol::protected_function>("DeregisterChild")(EnvironmentID, ParentID, ChildID); };

		template<typename T>
		static void RegisterComponent(int EnvironmentID, const std::string& EntityID, T* LinkedComponent) 
		{
			LuaVM.get<sol::protected_function>("RegisterComponent")(EnvironmentID, EntityID, TYPEID_STRING(T), LinkedComponent);
		};

		template<typename T>
		static void DeregisterComponent(int SceneID, const std::string& EntityID) 
		{
			LuaVM.get<sol::protected_function>("DeregisterComponent")(SceneID, EntityID, TYPEID_STRING(T));
		};

		static void ResetEntity(int EnvironmentID, Ptr<Entity> entity);

		template<typename... Args>
		static sol::object CallScene(int SceneID, const std::string& Name, Args&&... args)
		{
			return LuaVM.get<sol::protected_function>("CallScene")(SceneID, Name, std::forward<Args>(args)...);
		}

		static sol::table GetEntityBinding(int EnvironmentID, const std::string& EntityID)
		{
			return LuaVM.get<sol::protected_function>("GetEntityBinding")(EnvironmentID, EntityID);
		};

		static sol::table GetSceneBinding(int EnvironmentID)
		{
			return LuaVM.get<sol::protected_function>("GetSceneBinding")(EnvironmentID);
		}

		static inline LuaFunction GetCachedScript(const std::string& Name) { return CachedScripts[Name]; };
		static sol::state& GetGlobalEnvironment() { return LuaVM; };

	private:
		static void LoadGlobalEnvironment();
		
		static std::unordered_map<std::string, LuaFunction> CachedScripts;
		static sol::state LuaVM;
	};

}