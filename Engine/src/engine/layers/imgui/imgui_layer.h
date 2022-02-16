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

		void SetAbsorbInputs(bool Mode) { AbsorbInputs = Mode; };
		inline bool GetAbsorbInputs() const { return AbsorbInputs; };

	private:
		bool AbsorbInputs = true;
	};


}