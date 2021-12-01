#pragma once

#include "engine/layers/layer_set.h"

#include "engine/application/window.h"

namespace Techless {

	struct RuntimeInfo
	{
		unsigned int Framerate = 0;
		unsigned int UpdateRate = 0;
	};

	class Application {
	public:
		Application() = default;
		virtual ~Application() = default;

		void AddLayer(Layer* NewLayer);
		void AddOverlay(Layer* NewOverlay);

		void PushEvent(const InputObject& InputEvent);

		void Init();
		void Run();
		void End();

	public:
		LayerSet Layers;

		inline bool IsRunning() const { return Running; };
		inline Window* GetActiveWindow() const { return aWindow; };
	
		static Application& GetActiveApplication() { return *CurrentApplication; };
		static RuntimeInfo& GetRuntimeData() { return RuntimeData; }

		inline void SetApplicationTitle(const std::string& nApplicationTitle) { ApplicationTitle = nApplicationTitle; };

	private:
		static Application* CurrentApplication;
		static RuntimeInfo RuntimeData;

		static std::string ApplicationTitle;

		Window* aWindow;
		bool Running;
	};

}