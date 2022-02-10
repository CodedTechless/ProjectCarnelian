#pragma once

#include <engine/layers/layer_set.h>
#include <engine/layers/imgui/imgui_layer.h>
#include <engine/application/window.h>



#include "event.h"

namespace Techless {

	struct RuntimeInfo
	{
		unsigned int Framerate = 0;
		unsigned int UpdateRate = 0;

		float UpdateTime = 0;
		float FixedUpdateTime = 0;
	};

	class Application {
	public:
		Application() = default;
		~Application();
		
		static Application& GetActiveApplication() { return *CurrentApplication; };
		static RuntimeInfo& GetRuntimeData() { return RuntimeData; }

		inline bool IsRunning() const { return Running; };
		inline Window* GetActiveWindow() const { return aWindow; };
		inline void SetApplicationTitle(const std::string& nApplicationTitle) { ApplicationTitle = nApplicationTitle; };

		void AddLayer(Layer* NewLayer);
		void AddOverlay(Layer* NewOverlay);

	protected:
		void Init();
		void Run();
		void End();

		LayerSet Layers;

	private:
		void PushInputEvent(const InputEvent& inputEvent);
		void PushWindowEvent(const WindowEvent& windowEvent);

		ImGuiLayer* a_ImGuiLayer = nullptr;

	private:
		static Application* CurrentApplication;
		static RuntimeInfo RuntimeData;

		std::string ApplicationTitle;

		Window* aWindow = nullptr;
		bool Running = false;

		friend class Window;
	};

}