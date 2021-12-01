#pragma once

#include "engine/input/input.h"
#include <engineincl.h>

namespace Techless {
	class Layer {
	public:
		Layer(const std::string& LayerName = "Layer")
			: Name(LayerName) {};
		virtual ~Layer() = default;

		virtual void OnCreated() {};
		virtual void OnRemoved() {};

		virtual void OnUpdate(const float& Delta) {};		  // runs every frame
		virtual void OnUpdateEnd(const float& Delta) {};

		virtual void OnUpdateFixed(const float& Delta) {};	  // runs 60 times a second
		virtual void OnUpdateFixedEnd(const float& Delta) {}; // runs 60 times a second, after OnUpdateFixed.

		virtual bool OnInput(const InputObject& InputEvent) { return false; };
	private:
		std::string Name;
	};
}