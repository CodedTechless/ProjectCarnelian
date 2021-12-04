#include "application.h"

#include <engineincl.h>

#include <imgui/imgui.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/watchdog/watchdog.h>
#include <engine/input/input.h>

#include <render/renderer.h>

namespace Techless {

    Application* Application::CurrentApplication = nullptr;
    RuntimeInfo Application::RuntimeData = {};

    std::string Application::ApplicationTitle = "Application";

    constexpr float UpdateRate = 1.f / 60.f;

    void Application::Init() 
    {
        assert(CurrentApplication == nullptr);
        CurrentApplication = this;

        Debug::Log("Starting engine...", "Application");

        aWindow = new Window(ApplicationTitle.c_str(), { 1280, 720 });
        aWindow->SetVsyncEnabled(false);

        Renderer::Init();
        SpriteAtlas::Init();
    }

    void Application::End()
    {
        Running = false;
    }

    void Application::PushInputEvent(const InputEvent& inputEvent)
    {
        bool Processed = false;

        for (auto i = Layers.rbegin(); i != Layers.rend(); ++i)
        {
            Input::Filter Response = (*i)->OnInputEvent(inputEvent, Processed);

            if (Response == Input::Filter::Stop)
                break;
            else if (Response == Input::Filter::Continue)
                Processed = true;
        }
    }

    void Application::PushWindowEvent(const WindowEvent& windowEvent)
    {
        for (auto Layer : Layers)
        {
            Layer->OnWindowEvent(windowEvent);
        }
    }

    void Application::AddLayer(Layer* NewLayer)
    {
        Layers.PushLayer(NewLayer);
    }

    void Application::AddOverlay(Layer* NewOverlay)
    {
        Layers.PushOverlay(NewOverlay);
    }

    void Application::RenderDebugImGuiElements()
    {
        auto Runtime = RuntimeData;
        auto DebugInfo = Renderer::GetDebugInfo();

        {
            ImGui::Begin("Render Debug");

            if (ImGui::CollapsingHeader("Performance"))
            {
                ImGui::Columns(2, "performance_table");

                std::string PerformanceLabels = "FPS\nUpdate Rate\nLast fixed update took\nLast frame took";
                ImGui::Text(PerformanceLabels.c_str());

                ImGui::NextColumn();

                std::string PerformanceData = std::to_string(Runtime.Framerate) + "\n" + std::to_string(Runtime.UpdateRate) + "\n" + std::to_string(Runtime.FixedUpdateTime) + "ms\n" + std::to_string(Runtime.UpdateTime) + "ms";
                ImGui::Text(PerformanceData.c_str());

                ImGui::Columns();
            }

            if (ImGui::CollapsingHeader("Renderer Information"))
            {

                ImGui::Columns(2, "renderer_info_table");

                std::string RendererLabels = "Draw calls last frame\nVertex count last frame";
                ImGui::Text(RendererLabels.c_str());

                ImGui::NextColumn();

                std::string RendererData = std::to_string(DebugInfo.DrawCalls) + " calls\n" + std::to_string(DebugInfo.VertexCount) + " verticies";
                ImGui::Text(RendererData.c_str());

                ImGui::Columns();
            }

            ImGui::End();
        }
    }

    void Application::Run()
    {
        Running = true;

        float StartTime = (float)glfwGetTime();

        float Delta = 0.f;
        float Time = 0.f;

        float LastTime = StartTime;
        float LastFixedTime = StartTime;
        float Timer = LastTime;

        unsigned int Frames = 0, Updates = 0;

        while (Running)
        {
            Time = (float)glfwGetTime();
            
            auto FrameDelta = (Time - LastTime) / UpdateRate;
            Delta += FrameDelta;
            
            // fixed physics step.
            while (Delta >= 1.0)
            {
                RuntimeData.FixedUpdateTime = (Time - LastFixedTime) * 1000.f;

                for (auto* Layer : Layers)
                {
                    Layer->OnUpdateFixed(Delta);
                }

                for (auto* Layer : Layers) {
                    Layer->OnUpdateFixedEnd(Delta);
                }

                Updates++;
                Delta -= 1;

                LastFixedTime = Time;
            }

            aWindow->Clear();

            // unfixed rendering step.
            for (auto* Layer : Layers) {
                Layer->OnUpdate(FrameDelta);
            }

            for (auto* Layer : Layers) {
                Layer->OnUpdateEnd(FrameDelta);
            }
            Frames++;

            if (Time - Timer > 1.f)
            {
                Timer++;
                //std::cout << "FPS: " << Frames << " Fixed Updates: " << Updates << std::endl;

                RuntimeData.Framerate = Frames;
                RuntimeData.UpdateRate = Updates;
                
                Frames = 0;
                Updates = 0;
            }
            
            RuntimeData.UpdateTime = (Time - LastTime) * 1000.f;

            RenderDebugImGuiElements();
            aWindow->Update();      // swap the front and back buffers

            LastTime = Time;
        }

        glfwTerminate();
    }
}