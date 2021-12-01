#pragma once

#include "Engine.h"

using namespace Techless;

namespace DesecratedDungeons {
	class MainLayer : public Layer {
	public:
		void OnCreated();

		void OnUpdateFixed(const float& Delta);

		void OnUpdate(const float& Delta);
		void OnUpdateEnd(const float& Delta);

	private:
		std::shared_ptr<Scene> ActiveScene;

		float UpdateRate;
		float FixedUpdateRate;
	};
}