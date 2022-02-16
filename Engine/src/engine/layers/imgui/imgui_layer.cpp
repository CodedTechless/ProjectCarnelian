

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include "imgui_layer.h"

namespace Techless
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer") {}

	void ImGuiLayer::OnCreated()
	{
		// Set up ImGui for debug UI and shit like that
		IMGUI_CHECKVERSION();											// check the currently running version
		ImGui::CreateContext();											// create the ImGui context

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();										// sets the window colour style to dark mode

		ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);	// bind it to the currently active GLFW window
		ImGui_ImplOpenGL3_Init((char*)glGetString(330));				// initialises it in OpenGL mode
	}

	void ImGuiLayer::OnRemoved()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}