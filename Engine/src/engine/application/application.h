#pragma once

#include "layers/layer_set.h"
#include "layers/imgui/imgui_layer.h"

#include "window.h"
#include "event.h"

namespace Techless {

	struct RuntimeInfo
	{
		unsigned int Framerate = 0;
		unsigned int SimulationRate = 0;

		size_t LuaMemoryUsage = 0;

		float FrameDelta = 0;
		float SimulationDelta = 0;
	};

	class Application 
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		inline void SetApplicationTitle(const std::string& nApplicationTitle) { ApplicationTitle = nApplicationTitle; };

		void AddLayer(Layer* NewLayer);
		void AddOverlay(Layer* NewOverlay);

		inline bool IsRunning() const { return Running; };
		inline Window* GetActiveWindow() const { return aWindow; };

		inline ImGuiLayer* GetImGuiLayer() const { return a_ImGuiLayer; };

		inline float GetSimulationSpeed() const { return SimulationSpeed; };
		inline float GetSimulationRatio() const { return SimulationRatio; };
		
		static Application& GetActiveApplication() { return *CurrentApplication; };
		static RuntimeInfo& GetRuntimeData() { return RuntimeData; };

	protected:
		void Init();
		void Run();
		void End();

		LayerSet Layers;

	private:
		void PushInputEvent(const InputEvent& inputEvent);
		void PushWindowEvent(const WindowEvent& windowEvent);

		ImGuiLayer* a_ImGuiLayer = nullptr;

		float SimulationRatio = 0.f;
		float SimulationSpeed = 1.f / 60.f;

	private:
		static Application* CurrentApplication;
		static RuntimeInfo RuntimeData;

		std::string ApplicationTitle;

		Window* aWindow = nullptr;
		bool Running = false;

		friend class Window;
	};

}