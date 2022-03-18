#pragma once

#include <engine/application/layers/layer.h>

namespace Techless
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(const std::string& iniFileName = "imgui.ini");

		void OnCreated();
		void OnRemoved();

		void Begin();
		void End();

		void SetAbsorbInputs(bool Mode) { AbsorbInputs = Mode; };
		inline bool GetAbsorbInputs() const { return AbsorbInputs; };

	private:
		bool AbsorbInputs = true;

		std::string IniFileName = "imgui.ini";
	};


}