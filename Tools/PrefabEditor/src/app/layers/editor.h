#pragma once

#include <scriptindex.h>

#include <app/layers/editor_panels/explorer.h>
#include <app/layers/editor_panels/asset_manager.h>

#include "Engine.h"

using namespace Techless;

namespace PrefabEditor {

	class Editor : public Layer 
	{
	public:
		void OnCreated();

		void OnUpdate(const float& Delta);
		void OnUpdateEnd(const float& Delta);
		void OnUpdateFixed(const float& Delta);

		Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed);
		//void OnWindowEvent(const WindowEvent& windowEvent);

	private:
		ExplorerPanel EditorExplorer;
		AssetManagerPanel EditorAssetManager;

		std::string PrefabName = "New Prefab";

	private:
		Ptr<Scene> ActiveScene;
		Ptr<NativeScript::Core::Camera> ActiveCameraScript;
		Ptr<FrameBuffer> ActiveFrameBuffer;

		glm::u32vec2 ViewportSize;
		bool ViewportFocused = false;

		Entity* SelectedEntity = nullptr;

		float UpdateRate;
		float FixedUpdateRate;
	};
}