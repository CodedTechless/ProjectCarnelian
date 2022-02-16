#pragma once

#include <Engine.h>
#include <engine/watchdog/imgui/console/console_panel.h>

using namespace Techless;

namespace Carnelian {

	class Core : public Layer 
	{
	public:
		void OnCreated();

		void OnUpdate(const float Delta);
		void OnUpdateFixed(const float Delta);
		void OnUpdateEnd(const float Delta);

		Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed);
		void OnWindowEvent(const WindowEvent& windowEvent);

	private:
		Ptr<Scene> ActiveScene = nullptr;

		ExplorerPanel SceneExplorer{};
		ConsolePanel SceneConsole{};
		
		float UpdateRate;
		float FixedUpdateRate;

	private:
		void BuildEnvironment();

		sol::protected_function GetFunction(const std::string& Name)
		{
			return LayerScript->get<sol::protected_function>(Name);
		}

		Ptr<LuaEnv> LayerScript = nullptr;

	};

}