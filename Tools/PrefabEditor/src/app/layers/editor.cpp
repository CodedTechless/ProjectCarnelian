
#include <Engine.h>
#include <scriptindex.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <imgui/imgui.h>

#include "editor.h"

using namespace Techless;

namespace Carnelian {

	void Editor::OnCreated()
	{
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ActiveScene = CreatePtr<Scene>();

        auto& SceneCamera = ActiveScene->CreateEntity();
        SceneCamera.AddComponent<TransformComponent>();
        SceneCamera.AddComponent<CameraComponent>();

        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        Script.Bind<Camera>(SceneCamera);

        ActiveScene->SetActiveCamera(SceneCamera);

        Renderer::SetClearColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	}

    void Editor::OnUpdateFixed(const float& Delta)
    {
        ActiveScene->FixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Editor::OnUpdate(const float& Delta)
    {
        ActiveScene->Update(Delta);

        UpdateRate = Delta;
    }

    void Editor::OnUpdateEnd(const float& Delta)
    {
        ImGui::Begin("My First Tool", nullptr, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    Input::Filter Editor::OnInputEvent(const InputEvent& inputEvent, bool Processed)
    {
        return ActiveScene->OnInputEvent(inputEvent, Processed);
    }

    void Editor::OnWindowEvent(const WindowEvent& windowEvent)
    {
        ActiveScene->OnWindowEvent(windowEvent);
    }
}