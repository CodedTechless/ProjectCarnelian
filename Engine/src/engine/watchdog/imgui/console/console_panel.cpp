#include "console_panel.h"

#include <imgui/imgui.h>

namespace Techless
{

	void ConsolePanel::RenderImGuiElements()
	{

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 450.f, 200.f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5.f, 5.f });
		ImGui::Begin("Console");
		ImGui::PopStyleVar(1);

		char buf[50] = {};

		ImVec2 Size = ImGui::GetContentRegionAvail();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f, 0.f, 0.f, 0.2f });
		ImGui::BeginChildFrame(1, { Size.x, Size.y - 24 }, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PopStyleColor();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 1 });

		for (const auto& LogItem : Debug::GetMessageLog())
		{
			ImVec4 Colour = { 0.925f, 0.925f, 0.925f, 1.f };

			if (LogItem.Header == "Lua")
				Colour = { 0.424f, 0.663f, 0.941f, 1.f };
			else if (LogItem.Header == "OpenGL")
				Colour = { 0.961f, 0.886f, 0.643f, 1.f };
			else if (LogItem.Type == "ERROR")
				Colour = { 0.95f, 0.45f, 0.45f, 1.f };
			else if (LogItem.Type == "WARN")
				Colour = { 0.961f, 0.729f, 0.404f, 1.f };

			ImGui::PushStyleColor(ImGuiCol_Text, Colour);

			std::string str = "[" + LogItem.Type + "][" + LogItem.Header + "] " + LogItem.Contents;
			ImGui::TextUnformatted(str.c_str());
			ImGui::PopStyleColor();
		}

		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.f);

		ImGui::PopStyleVar();
		ImGui::EndChildFrame();

		ImGui::PushItemWidth(Size.x);
		ImGui::InputText("##amongus", buf, 50);
		ImGui::PopItemWidth();

		ImGui::End();


	}

}