
#include <Engine.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <imgui/imgui.h>

#include "editor.h"

using namespace Techless;

namespace PrefabEditor {

	void Editor::OnCreated()
	{
        ActiveScene = CreatePtr<Scene>();


        auto& SceneCamera = ActiveScene->CreateEntity("techless_EditorCamera");
        SceneCamera.AddComponent<CameraComponent>();
        SceneCamera.Archivable = false;

        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        ActiveCameraScript = Script.Bind<NativeScript::Core::Camera>(SceneCamera);

        ActiveScene->SetActiveCamera(SceneCamera);
        
        FrameBufferSpecification FSpec;
        FSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth };
        FSpec.Size = { 128, 72 };
        ActiveFrameBuffer = CreatePtr<FrameBuffer>(FSpec);

        EditorExplorer.SetSceneContext(ActiveScene);
        EditorExplorer.Refresh();
    }

    void Editor::OnUpdateFixed(const float& Delta)
    {
        ActiveCameraScript->OnFixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Editor::OnUpdate(const float& Delta)
    {
        auto Spec = ActiveFrameBuffer->GetSpecification();
        if ((Spec.Size.x != ViewportSize.x or Spec.Size.y != ViewportSize.y) 
            and (ViewportSize.x > 0 and ViewportSize.y > 0))
        {
            ActiveFrameBuffer->Resize(ViewportSize);
            ActiveCameraScript->OnWindowEvent({ ViewportSize });
        }

        ActiveCameraScript->OnUpdate(Delta);

        ActiveFrameBuffer->Bind();
        Renderer::SetClearColour({ 0.1f, 0.1f, 0.1f, 1.f });
        Renderer::Clear();

        ActiveScene->Update(Delta, false);
        ActiveFrameBuffer->Unbind();
        
        UpdateRate = Delta;
    }

    void Editor::OnUpdateEnd(const float& Delta)
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::Begin("Editor", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) 
                {
                    ActiveScene->Serialise("assets/prefabs/" + PrefabName + ".prefab");

                    Debug::Log("Saved prefab!", "PrefabEditor");
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Close", "Ctrl+W", nullptr, false)) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Insert"))
            {
                if (ImGui::MenuItem("Create Entity")) 
                {
                    auto& Entity = ActiveScene->CreateEntity();

                    auto& c_Sprite = Entity.AddComponent<SpriteComponent>();
                    c_Sprite.SetSprite("checkers");

                    EditorExplorer.SetSelectedEntity(Entity);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        ImGui::End();

        // -= Viewport =-
        
        ImGui::Begin("Viewport");

        {
            ViewportFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
            ActiveCameraScript->AcceptingInput = ImGui::IsWindowFocused();

            ImVec2 Region = ImGui::GetContentRegionAvail();
            ViewportSize = { Region.x, Region.y };

            uint32_t rID = ActiveFrameBuffer->GetColourAttachmentRendererID();
            ImGui::Image((ImTextureID)rID, ImVec2{ (float)ViewportSize.x, (float)ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }

        ImGui::End();

        // -= Project Settings =-

        ImGui::Begin("Prefab Settings");

        {
            char buf[50] = {};
            strcpy_s(buf, PrefabName.c_str());

            ImGui::InputText("Prefab Name", buf, 50);

            PrefabName = buf;
        }

        ImGui::End();

        // ------------

        EditorExplorer.RenderImGuiElements();
        EditorAssetManager.RenderImGuiElements();

        Renderer::ShowRuntimeStatsWindow();
    }

    Input::Filter Editor::OnInputEvent(const InputEvent& inputEvent, bool Processed)
    {
        if (ViewportFocused)
            return ActiveCameraScript->OnInputEvent(inputEvent, Processed);
    
        return Input::Filter::Ignore;
    }

    /*
    void Editor::OnWindowEvent(const WindowEvent& windowEvent)
    {
        ActiveScene->OnWindowEvent(windowEvent);
    }
    */
}