#pragma once

#include "Engine.h"

using namespace Techless;

namespace TextureEditor {
	class MainLayer : public Layer {
	public:
		void OnCreated();

		//void OnUpdateFixed(const float& Delta);

		void OnUpdate(const float& Delta);
		void OnUpdateEnd(const float& Delta);

		bool OnInput(const InputObject& InputEvent);

	private:
		float UpdateRate;
	};
}