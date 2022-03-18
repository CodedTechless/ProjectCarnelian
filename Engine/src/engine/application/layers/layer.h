#pragma once

#include <engineincl.h>

#include <engine/application/event.h>

// to-do: integrate lua with layer system!

namespace Techless {


	class Layer 
	{
	public:
		Layer() = default;
		Layer(const std::string& layerName);
		virtual ~Layer() = default;

		virtual void OnCreated() {};
		virtual void OnRemoved() {};

		virtual void OnUpdate(const float Delta) {};
		virtual void OnUpdateEnd(const float Delta) {};

		virtual void OnUpdateFixed(const float Delta) {};
		virtual void OnUpdateFixedEnd(const float Delta) {};

		virtual Input::Filter OnInputEvent(InputEvent inputEvent, bool Processed) { return Input::Filter::Ignore; };
		virtual void OnWindowEvent(WindowEvent inputEvent) {};

	protected:
		std::string LayerName = "Layer";

	};
}