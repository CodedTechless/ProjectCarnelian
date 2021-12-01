
#include <engineincl.h>

#include "engine/sprite/sprite_atlas.h"
#include "engine/watchdog/watchdog.h"
#include "render/renderer.h"
#include "application.h"
#include "engine/input/input.h"

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

        auto Context = aWindow->GetContext();
        glfwSetWindowUserPointer(Context, this);

        glfwSetWindowCloseCallback(Context, 
            [](GLFWwindow* window)
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                App.End();
            });

        glfwSetWindowSizeCallback(Context,
            [](GLFWwindow* window, int Width, int Height)
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                App.GetActiveWindow()->Size = glm::uvec2(Width, Height);

                //std::cout << Width << " " << Height;
            });

        glfwSetFramebufferSizeCallback(Context,
            [](GLFWwindow* window, int Width, int Height)
            {
                glViewport(0, 0, Width, Height);
            });

        glfwSetScrollCallback(Context, 
            [](GLFWwindow* window, double xOffset, double yOffset) 
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                InputObject NewInput = { Input::GetMousePosition(), yOffset };
                App.PushEvent(NewInput);
            });

        glfwSetCursorPosCallback(Context, [](GLFWwindow* window, double xPos, double yPos)
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                InputObject NewInput = { {xPos, yPos}, 0.f };
                App.PushEvent(NewInput);
            });

        

        Renderer::Init();
        SpriteAtlas::Init();
    }

    void Application::End()
    {
        Running = false;
    }

    void Application::PushEvent(const InputObject& InputEvent)
    {
        for (auto i = Layers.rbegin(); i != Layers.rend(); ++i)
        {
            bool Response = (*i)->OnInput(InputEvent);

            if (Response)
                break;
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

        float Delta = 0.f;
        float Time = 0.f, LastTime = (float)glfwGetTime();
        float Timer = LastTime;

        unsigned int Frames = 0, Updates = 0;

        while (Running)
        {
            float Time = (float)glfwGetTime();

            auto FrameDelta = (Time - LastTime) / UpdateRate;
            Delta += FrameDelta;
            
            // fixed physics step.
            while (Delta >= 1.0)
            {
                for (auto* Layer : Layers)
                {
                    Layer->OnUpdateFixed(Delta);
                }

                for (auto* Layer : Layers) {
                    Layer->OnUpdateFixedEnd(Delta);
                }

                Updates++;

                Delta -= 1;
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
            
            aWindow->Update();      // swap the front and back buffers

            LastTime = Time;
        }

        glfwTerminate();
    }
}