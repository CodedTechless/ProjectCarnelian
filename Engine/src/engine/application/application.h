#pragma once

#include <engine/layers/layer_set.h>
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
		virtual ~Application() = default;

	public:
		inline bool IsRunning() const { return Running; };
		inline Window* GetActiveWindow() const { return aWindow; };
	
		static Application& GetActiveApplication() { return *CurrentApplication; };
		static RuntimeInfo& GetRuntimeData() { return RuntimeData; }

		inline void SetApplicationTitle(const std::string& nApplicationTitle) { ApplicationTitle = nApplicationTitle; };

	protected:
		void AddLayer(Layer* NewLayer);
		void AddOverlay(Layer* NewOverlay);

		void Init();
		void Run();
		void End();

		void PushInputEvent(const InputEvent& inputEvent);
		void PushWindowEvent(const WindowEvent& windowEvent);

		LayerSet Layers;

	private:
		void RenderDebugImGuiElements();

	private:
		static Application* CurrentApplication;
		static RuntimeInfo RuntimeData;

		static std::string ApplicationTitle;

		Window* aWindow;
		bool Running;

		friend class Window;
	};

}