#pragma once

#include "Engine.h"

using namespace Techless;

namespace Carnelian {

	class Core : public Layer 
	{
	public:
		void OnCreated();

		void OnUpdate(const float& Delta);
		void OnUpdateFixed(const float& Delta);

	private:
		std::shared_ptr<Scene> ActiveScene;

		float UpdateRate;
		float FixedUpdateRate;
	};

}