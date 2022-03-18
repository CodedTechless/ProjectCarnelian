
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <imgui/imgui.h>

#include "editor.h"

using namespace Techless;

namespace PrefabEditor {

	void Editor::OnCreated()
	{
        Application::GetActiveApplication().GetImGuiLayer()->SetAbsorbInputs(false);

        CreateScene("New Prefab");
        SetScene("New Prefab");
    }
    
    void Editor::CreateScene(const std::string& SceneName, Prefab* LoadWithPrefab)
    {
        for (auto& scene : Scenes)
        {
            if (scene->SceneName == SceneName)
            {
                Debug::Log("Scene with name " + SceneName + " already exists.", "PrefabEditor");
                return;
            }
        }

        auto NewScene = CreatePtr<EditorScene>( SceneName, LoadWithPrefab );
        
        Scenes.push_back(NewScene);
    }

    void Editor::SetScene(const std::string& SceneName)
    {
        for (auto& scene : Scenes)
        {
            if (scene->SceneName == SceneName)
            {
                ActiveEditorScene = scene;
                EditorExplorer.SetSceneContext(ActiveEditorScene->LinkedScene);

                Debug::Log("Changed scene to " + SceneName, "PrefabEditor");
            }
        }
    }

    void Editor::OnUpdateFixed(const float Delta)
    {
        ActiveEditorScene->ActiveCameraScript->OnFixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Editor::OnUpdate(const float Delta)
    {
        auto& Camera = ActiveEditorScene->LinkedScene->GetActiveCamera();
        auto& c_Camera = Camera.GetComponent<CameraComponent>();

        if ((c_Camera.GetViewport().Size.x != ViewportSize.x or c_Camera.GetViewport().Size.y != ViewportSize.y)
            and (ViewportSize.x > 0 and ViewportSize.y > 0))
        {
            c_Camera.SetViewportSize(ViewportSize);
            ActiveEditorScene->ActiveCameraScript->OnWindowEvent({ ViewportSize });
        }

        ActiveEditorScene->ActiveCameraScript->OnUpdate(Delta);
        ActiveEditorScene->LinkedScene->Update(Delta);
        
        UpdateRate = Delta;
    }

    void Editor::OnUpdateEnd(const float Delta)
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
                if (ImGui::MenuItem("New")) 
                { 
                    CreateScene("new " + std::to_string(++NewScenes)); 
                };
                
                if (ImGui::MenuItem("Save", "Ctrl+S")) 
                {
                    ActiveEditorScene->Save();

                    Debug::Log("Saved prefab!", "PrefabEditor");
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Close", "Ctrl+W", nullptr, false))
                {

                };

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Insert"))
            {
                if (ImGui::MenuItem("Create Entity")) 
                {
                    auto& Entity = ActiveEditorScene->LinkedScene->CreateEntity();
                    Entity.SetParent(ActiveEditorScene->SceneRoot);

                    EditorExplorer.SetSelectedEntity(&Entity);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        ImGui::End();

        // -= Viewport =-
        
        ImGui::Begin("Viewport");

        {
            ImGui::BeginTabBar("##scene selector", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable);

            for (auto& scene : Scenes)
            {
                if (ImGui::BeginTabItem(scene->SceneName.c_str(), nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
                {
                    if (ActiveEditorScene->SceneName != scene->SceneName)
                        SetScene(scene->SceneName);
                    
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }

        {
            auto& Camera = ActiveEditorScene->LinkedScene->GetActiveCamera();
            auto& c_Camera = Camera.GetComponent<CameraComponent>();

            ViewportFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
            ActiveEditorScene->ActiveCameraScript->AcceptingInput = ImGui::IsWindowFocused();

            ImVec2 Region = ImGui::GetContentRegionAvail();
            ViewportSize = { Region.x, Region.y };

            ImGui::PushID("ViewportDropZone");

            uint32_t rID = c_Camera.GetFrameBuffer()->GetColourAttachmentRendererID();

            ImVec2 CursorScreenPos = ImGui::GetCursorScreenPos();
            c_Camera.SetViewportPosition({ CursorScreenPos.x, CursorScreenPos.y });
            ImGui::Image((ImTextureID)rID, ImVec2{ (float)ViewportSize.x, (float)ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            

            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("PREFAB_ASSET_DRAG");

                if (Payload)
                {
                    std::string PrefabPath = (const char*)Payload->Data;

                    Debug::Log(PrefabPath);

                    auto& Prefab = PrefabAtlas::Get(PrefabPath);

                    Debug::Log(Prefab.GetName());
                    CreateScene(Prefab.GetName(), &Prefab);
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::PopID();
        }

        ImGui::End();

        // -= Project Settings =-

        ImGui::Begin("Prefab Settings");

        {
            char buf[50] = {};
            strcpy_s(buf, ActiveEditorScene->SceneName.c_str());

            ImGui::InputText("Prefab Name", buf, 50);

            ActiveEditorScene->SceneName = buf;
        }

        ImGui::End();

        // ------------

        EditorExplorer.RenderImGuiElements();
        EditorAssetManager.RenderImGuiElements();
        EditorConsole.RenderImGuiElements();

        Renderer::ShowRuntimeStatsWindow();
    }

    Input::Filter Editor::OnInputEvent(InputEvent inputEvent, bool Processed)
    {
        if (ViewportFocused)
        {
            if (inputEvent.InputType == Input::Type::Mouse)
            {
                auto& Camera = ActiveEditorScene->LinkedScene->GetActiveCamera();
                auto& c_Camera = Camera.GetComponent<CameraComponent>();

                Viewport viewport = c_Camera.GetViewport();
                inputEvent.Position -= Vector3(viewport.Position, 0.f);

                if (inputEvent.Position.x > viewport.Size.x || inputEvent.Position.y > viewport.Size.y || inputEvent.Position.x < 0 || inputEvent.Position.y < 0)
                {
                    return Input::Filter::Ignore;
                }
            }

            return ActiveEditorScene->ActiveCameraScript->OnInputEvent(inputEvent, Processed);
        }

        return Input::Filter::Ignore;
    }

    /*
    void Editor::OnWindowEvent(const WindowEvent& windowEvent)
    {
        ActiveScene->OnWindowEvent(windowEvent);
    }
    */
}