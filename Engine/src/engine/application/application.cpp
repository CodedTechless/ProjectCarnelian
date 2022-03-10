#include "application.h"

#include <engineincl.h>

#include <imgui/imgui.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/sprite/animation/animation_atlas.h>

#include <engine/lua/script_environment.h>
#include <render/renderer.h>

#include <engine/application/watchdog/watchdog.h>
#include <engine/application/input/input.h>

namespace Techless {

    Application* Application::CurrentApplication = nullptr;
    RuntimeInfo Application::RuntimeData = {};


    void Application::Init() 
    {


        assert(CurrentApplication == nullptr);
        CurrentApplication = this;

        Debug::Log("Starting engine...", "Application");

        aWindow = new Window(ApplicationTitle.c_str(), { 1280, 720 });
        aWindow->SetVsyncEnabled(false);

        Renderer::Init();
        SpriteAtlas::Init();
        ScriptEnvironment::Init();
        AnimationAtlas::Init();

        a_ImGuiLayer = new ImGuiLayer();
        Layers.PushOverlay(a_ImGuiLayer);
    }

    void Application::End()
    {
        ScriptEnvironment::End();
        Running = false;
    }

    void Application::PushInputEvent(const InputEvent& inputEvent)
    {
        //Debug::Log("Event " + std::to_string((int)inputEvent.InputType) + " " + std::to_string((int)inputEvent.InputState) + " " + std::to_string((int)inputEvent.KeyCode) + " " + std::to_string((int)inputEvent.MouseCode) + " (" + std::to_string(inputEvent.Delta.x) + ", " + std::to_string(inputEvent.Delta.y) + ") (" + std::to_string(inputEvent.Position.x) + ", " + std::to_string(inputEvent.Position.y) + ")", "Application");

        if (a_ImGuiLayer->GetAbsorbInputs())
        {
            ImGuiIO& io = ImGui::GetIO();
            if (((inputEvent.InputType == Input::Type::Mouse || inputEvent.InputType == Input::Type::Scrolling) && io.WantCaptureMouse) ||
                (inputEvent.InputType == Input::Type::Keyboard && io.WantCaptureKeyboard))
            {
                return;
            }
        }

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

            float SimulationTime = (Time - LastFixedTime);
            float FrameTime = (Time - LastTime);

            float FrameDelta = FrameTime / SimulationSpeed;
            Delta += FrameDelta;


            while (Delta >= 1.0)
            {
                RuntimeData.SimulationDelta = SimulationTime * 1000.f;

                for (auto* Layer : Layers)
                {
                    Layer->OnUpdateFixed(SimulationTime);
                }

                for (auto* Layer : Layers) {
                    Layer->OnUpdateFixedEnd(SimulationTime);
                }

                Updates++;
                Delta -= 1;

                LastFixedTime = Time;
            }

            SimulationRatio = std::clamp(Delta, 0.f, 1.f);

            {
                aWindow->Clear();
                a_ImGuiLayer->Begin();

                for (auto* Layer : Layers) {
                    Layer->OnUpdate(FrameTime);
                }

                for (auto* Layer : Layers) {
                    Layer->OnUpdateEnd(FrameTime);
                }
                Frames++;

                a_ImGuiLayer->End();
                aWindow->Update();
            }

            if (Time - Timer > 1.f)
            {
                ScriptEnvironment::Clean();

                Timer++;

                RuntimeData.Framerate = Frames;
                RuntimeData.SimulationRate = Updates;
                
                Frames = 0;
                Updates = 0;
            }

            RuntimeData.LuaMemoryUsage = ScriptEnvironment::GetMemoryUsage();
            
            RuntimeData.FrameDelta = FrameTime * 1000.f;

            LastTime = Time;
        }

        
    }
}