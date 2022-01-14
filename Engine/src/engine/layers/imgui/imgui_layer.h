#pragma once

#include <engine/layers/layer.h>

namespace Techless
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		void OnCreated();
		void OnRemoved();

		void Begin();
		void End();
	};


}