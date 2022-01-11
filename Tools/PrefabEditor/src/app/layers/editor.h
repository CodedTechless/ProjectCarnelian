#pragma once

#include "Engine.h"

using namespace Techless;

namespace Carnelian {

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
		
		void RenderViewport();
		void RenderSceneHierarchy();
		void RenderProperties();
		void RenderAssetManager();

	private:
		Ptr<Scene> ActiveScene;
		Ptr<FrameBuffer> ActiveFrameBuffer;

		glm::u32vec2 ViewportSize;

		Entity* SelectedEntity = nullptr;

		float UpdateRate;
		float FixedUpdateRate;
	};

}