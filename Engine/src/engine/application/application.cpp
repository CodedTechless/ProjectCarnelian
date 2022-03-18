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


    void Application::Init(const std::string& ImGuiIniFileName)
    {
        assert(CurrentApplication == nullptr);
        CurrentApplication = this;

        Debug::Log("Starting engine...", "Application");

        aWindow = new Window(ApplicationTitle.c_str(), { 1280, 720 });
        aWindow->SetVsyncEnabled(true);

        Renderer::Init();
        ScriptEnvironment::Init();
        SpriteAtlas::Init();

        SpriteAtlas::Load("assets");
        AnimationAtlas::Load("assets");

        a_ImGuiLayer = new ImGuiLayer(ImGuiIniFileName);
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

        SimulationSpeed = 1.f / SimulationTickRate;

        float Time = 0.f;
        float CurrentTime = (float)glfwGetTime();
        float Accumulator = 0.f;

        size_t Frames = 0, Updates = 0;

        while (Running)
        {
            float NewTime = (float)glfwGetTime();
            float FrameTime = std::min(NewTime - CurrentTime, 0.25f);
            CurrentTime = NewTime;

            Time += FrameTime;
            Accumulator += FrameTime;

            while (Accumulator >= SimulationSpeed)
            {
                RuntimeData.SimulationDelta = Accumulator * 1000.f;
                Updates++;

                for (Layer* a_Layer : Layers) {
                    a_Layer->OnUpdateFixed(Accumulator);
                }

                for (Layer* a_Layer : Layers) {
                    a_Layer->OnUpdateFixedEnd(Accumulator);
                }

                Accumulator -= SimulationSpeed;
            }

            SimulationRatio = Accumulator / SimulationSpeed;

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
                
            size_t MemUsage = ScriptEnvironment::GetMemoryUsage();
            RuntimeData.LuaMemoryUsage = MemUsage;

            if (Time > 1.f)
            {
                Time--;

                if (MemUsage > 10000000)
                    ScriptEnvironment::Clean();

                RuntimeData.Framerate = Frames;
                RuntimeData.SimulationRate = Updates;

                Frames = 0; Updates = 0;
            }

            RuntimeData.FrameDelta = FrameTime * 1000.f;
        }

        
    }
}