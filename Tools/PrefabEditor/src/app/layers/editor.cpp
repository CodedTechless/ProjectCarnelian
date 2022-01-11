
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

        auto& SceneCamera = ActiveScene->CreateEntity("Camera");
        SceneCamera.RemoveComponent<TagComponent>();
        SceneCamera.AddComponent<CameraComponent>();

        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        Script.Bind<NativeScripts::Core::Camera>(SceneCamera);

        ActiveScene->SetActiveCamera(SceneCamera);

        FrameBufferSpecification FSpec;
        FSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth };
        FSpec.Size = { 128, 72 };

        ActiveFrameBuffer = CreatePtr<FrameBuffer>(FSpec);

        Renderer::SetClearColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
	}

    void Editor::OnUpdateFixed(const float& Delta)
    {
        //ActiveScene->FixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Editor::OnUpdate(const float& Delta)
    {

        //ActiveFrameBuffer->Resize(ViewportSize);
        ActiveFrameBuffer->Bind();
        
        /*
        auto CameraScript = ActiveScene->GetActiveCamera().GetComponent<ScriptComponent>().GetScript<Camera>();
        WindowEvent wEvent;
        wEvent.Size = ViewportSize;
        CameraScript->OnWindowEvent(wEvent);
        */

        ActiveScene->Update(Delta, false);

        ActiveFrameBuffer->Unbind();
        
        UpdateRate = Delta;
    }

    void Editor::OnUpdateEnd(const float& Delta)
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        
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
                    ActiveScene->Serialise("assets/prefabs/New Prefab.prefab");
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Close", "Ctrl+W", nullptr, false)) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Insert"))
            {
                if (ImGui::MenuItem("Create Entity", "")) 
                {
                    auto& Entity = ActiveScene->CreateEntity();
                    SelectedEntity = &Entity;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        ImGui::End();


        RenderViewport();
        RenderSceneHierarchy();
        RenderProperties();
        RenderAssetManager();
    }
    
    void Editor::RenderViewport()
    {
        ImGui::Begin("Viewport");

        auto rID = ActiveFrameBuffer->GetColourAttachmentRendererID();
        
        auto Region = ImGui::GetContentRegionAvail();
        ViewportSize = { Region.x, Region.y };

        ImGui::Image(reinterpret_cast<void*>(rID), ImVec2{ (float)ViewportSize.x, (float)ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();
    }

    void Editor::RenderSceneHierarchy()
    {
        ImGui::Begin("Explorer");
        
        auto Entities = ActiveScene->GetInstances<Entity>();

        auto Transforms = ActiveScene->GetInstances<TransformComponent>();
        auto Tags = ActiveScene->GetInstances<TagComponent>();

        int i = 0;
        for (auto& transform : *Transforms)
        {
            auto EntityID = Transforms->GetIDAtIndex(i++);
            if (!Tags->Has(EntityID))
                continue;

            auto& tag = Tags->Get(EntityID);

            ImGuiTreeNodeFlags node_flags = (SelectedEntity && SelectedEntity->GetID() == EntityID ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

            bool Opened = ImGui::TreeNodeEx(tag.Name.c_str(), node_flags);
            if (ImGui::IsItemClicked())
            {
                SelectedEntity = &Entities->Get(EntityID);
            }
        }
        ImGui::End();
    }

    void Editor::RenderProperties()
    {
        ImGui::Begin("Properties");

        ImGui::End();
    }

    void Editor::RenderAssetManager()
    {
        ImGui::Begin("Asset Manager");



        ImGui::End();
    }

    Input::Filter Editor::OnInputEvent(const InputEvent& inputEvent, bool Processed)
    {
        return ActiveScene->OnInputEvent(inputEvent, Processed);
    }

    /*
    void Editor::OnWindowEvent(const WindowEvent& windowEvent)
    {
        ActiveScene->OnWindowEvent(windowEvent);
    }
    */
}