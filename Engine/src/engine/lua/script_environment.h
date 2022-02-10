#pragma once

#include <sol/sol.hpp>
#include <engineincl.h>

namespace Techless
{
	class Entity;

	class ScriptEnvironment
	{
	public:

		static void Init();
		static Ptr<sol::environment> Create(const std::string& FilePath, Entity* entity);

		static sol::protected_function RegisterScene;
		static sol::protected_function RegisterEntity;
		
		static sol::protected_function DeregisterEntity;
		static sol::protected_function DeregisterScene;
		
		static sol::protected_function RegisterComponent;
		static sol::protected_function DeregisterComponent;

		static sol::protected_function GetEntityBinding;

	private:
		static std::unordered_map<std::string, sol::protected_function> CachedScripts;
		static void Read(const std::string& Path);

		static sol::state LuaVM;
	};

}