#pragma once

#include <scriptindex.h>

#include <engine/watchdog/imgui/explorer/explorer.h>
#include <engine/watchdog/imgui/console/console_panel.h>
#include <editor_panels/asset_manager.h>
#include <editor_scene/editor_scene.h>

using namespace Techless;

namespace PrefabEditor 
{

	class Editor : public Layer 
	{
	public:
		void OnCreated();

		void OnUpdate(const float Delta);
		void OnUpdateEnd(const float Delta);
		void OnUpdateFixed(const float Delta);

		Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed);
		//void OnWindowEvent(const WindowEvent& windowEvent);

		void CreateScene(const std::string& SceneName, Prefab* LoadWithPrefab = nullptr);
		void SetScene(const std::string& SceneName);

	private:
		ExplorerPanel EditorExplorer{};
		AssetManagerPanel EditorAssetManager{};

		ConsolePanel EditorConsole{};

	private:
		Ptr<EditorScene> ActiveEditorScene = nullptr;
		std::vector<Ptr<EditorScene>> Scenes{};
		
		int NewScenes = 1;

	private:
		Ptr<FrameBuffer> ActiveFrameBuffer = nullptr;
		
		glm::u32vec2 ViewportSize = { 0.f, 0.f };
		bool ViewportFocused = false;

		float UpdateRate = 0.f;
		float FixedUpdateRate = 0.f;
	};
}