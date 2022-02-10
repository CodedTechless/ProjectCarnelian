#include "application.h"

#include <engineincl.h>

#include <imgui/imgui.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/lua/script_environment.h>
#include <render/renderer.h>

#include <engine/watchdog/watchdog.h>
#include <engine/input/input.h>


#include <lua.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Techless {

    Application* Application::CurrentApplication = nullptr;
    RuntimeInfo Application::RuntimeData = {};

    constexpr float UpdateRate = 1.f / 60.f;

    Application::~Application()
    {
        glfwTerminate();
    }

    void Application::Init() 
    {

        sol::state test;
        test.open_libraries(sol::lib::base);

        test.script("print('bark bark bark!')");


        assert(CurrentApplication == nullptr);
        CurrentApplication = this;

        Debug::Log("Starting engine...", "Application");

        aWindow = new Window(ApplicationTitle.c_str(), { 1280, 720 });
        aWindow->SetVsyncEnabled(false);

        Renderer::Init();
        SpriteAtlas::Init();
        ScriptEnvironment::Init();

        a_ImGuiLayer = new ImGuiLayer();
        Layers.PushOverlay(a_ImGuiLayer);
    }

    void Application::End()
    {
        Running = false;
    }

    void Application::PushInputEvent(const InputEvent& inputEvent)
    {
        //Debug::Log("Event " + std::to_string((int)inputEvent.InputType) + " " + std::to_string((int)inputEvent.InputState) + " " + std::to_string((int)inputEvent.KeyCode) + " " + std::to_string((int)inputEvent.MouseCode) + " (" + std::to_string(inputEvent.Delta.x) + ", " + std::to_string(inputEvent.Delta.y) + ") (" + std::to_string(inputEvent.Position.x) + ", " + std::to_string(inputEvent.Position.y) + ")", "Application");

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
            
            auto FrameDelta = (Time - LastTime) / UpdateRate;
            Delta += FrameDelta;
            
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

            {
                aWindow->Clear();
                a_ImGuiLayer->Begin();

                for (auto* Layer : Layers) {
                    Layer->OnUpdate(FrameDelta);
                }

                for (auto* Layer : Layers) {
                    Layer->OnUpdateEnd(FrameDelta);
                }
                Frames++;

                a_ImGuiLayer->End();
                aWindow->Update();
            }

            if (Time - Timer > 1.f)
            {
                Timer++;

                RuntimeData.Framerate = Frames;
                RuntimeData.UpdateRate = Updates;
                
                Frames = 0;
                Updates = 0;
            }
            
            RuntimeData.UpdateTime = (Time - LastTime) * 1000.f;

            LastTime = Time;
        }

        
    }
}