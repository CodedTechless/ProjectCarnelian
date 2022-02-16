
#include "window.h"

#include <render/renderer.h>
#include <engine/application/application.h>

#include <engine/watchdog/watchdog.h>


namespace Techless 
{
	
    void GLFWErrorCallback(int error, const char* msg) {
        std::string s;

        s = " [" + std::to_string(error) + "] " + msg + '\n';
        std::cerr << s << std::endl;
    }

    struct UserPointerInfo
    {
        Application* App;
        Window* ActiveWindow;
    };

    Window::Window(const char AppTitle[], glm::vec2 aSize)
        : Size(aSize)
    {
        glfwSetErrorCallback(GLFWErrorCallback);

        if (glfwInit() != GL_TRUE)
            assert(false);

        /*
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        */


        // create the new active window
        aWindow = glfwCreateWindow(aSize.x, aSize.y, AppTitle, NULL, NULL);
        if (aWindow == nullptr) // if the window couldn't be created, terminate the app
        {
            glfwTerminate();
            assert(false);
        }

        glfwMakeContextCurrent(aWindow);
        if (glewInit() != GLEW_OK)
            assert(false);

        Debug::Log("Created window context (" + std::to_string(aSize.x) + ", " + std::to_string(aSize.y) + ")", "Window");

        // Set up all the input callbacks.
        UserPointerInfo* userPointerInfo = new UserPointerInfo({ &Application::GetActiveApplication(), this });

        glfwSetWindowUserPointer(aWindow, (void*)userPointerInfo);

        glfwSetWindowCloseCallback(aWindow,
            [](GLFWwindow* window)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);
                PointerInfo.App->End();
            });

        glfwSetWindowSizeCallback(aWindow,
            [](GLFWwindow* window, int Width, int Height)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);
                PointerInfo.ActiveWindow->Size = glm::uvec2(Width, Height);

                WindowEvent NewEvent{};

                NewEvent.Size = PointerInfo.ActiveWindow->Size;
                NewEvent.Focused = PointerInfo.ActiveWindow->IsFocused;

                PointerInfo.App->PushWindowEvent(NewEvent);
            });

        glfwSetWindowFocusCallback(aWindow,
            [](GLFWwindow* window, int Focused) 
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);

                WindowEvent NewEvent{};

                NewEvent.Size = PointerInfo.ActiveWindow->Size;
                NewEvent.Focused = (bool)Focused;

                PointerInfo.App->PushWindowEvent(NewEvent);
            });

        glfwSetScrollCallback(aWindow,
            [](GLFWwindow* window, double xOffset, double yOffset)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);

                InputEvent NewEvent{};

                NewEvent.InputState = Input::State::Changed;
                NewEvent.InputType = Input::Type::Scrolling;

                NewEvent.Delta = glm::vec3(xOffset, yOffset, 0.0);

                PointerInfo.App->PushInputEvent(NewEvent);
            });

        glfwSetCursorPosCallback(aWindow,
            [](GLFWwindow* window, double xPos, double yPos)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);

                InputEvent NewEvent{};

                NewEvent.InputState = Input::State::Changed;
                NewEvent.InputType = Input::Type::Mouse;

                NewEvent.Position = glm::vec3((int)xPos, (int)yPos, 0);
                
                //Debug::Log(std::to_string(xPos) + " " + std::to_string(yPos));

                PointerInfo.App->PushInputEvent(NewEvent);
            });
        
        glfwSetMouseButtonCallback(aWindow,
            [](GLFWwindow* window, int button, int action, int mods)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);

                InputEvent NewEvent{};

                NewEvent.InputType = Input::Type::Mouse;
                NewEvent.MouseCode = (Input::MouseCode)button;
                
                double X, Y;
                glfwGetCursorPos(window, &X, &Y);

                NewEvent.Position = glm::vec3((int)X, (int)Y, 0);

                switch (action)
                {
                    case GLFW_PRESS: NewEvent.InputState = Input::State::Begin; break;
                    case GLFW_RELEASE: NewEvent.InputState = Input::State::End; break;
                }

                PointerInfo.App->PushInputEvent(NewEvent);
            });

        glfwSetKeyCallback(aWindow,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);

                InputEvent NewEvent{};

                NewEvent.InputType = Input::Type::Keyboard;
                NewEvent.KeyCode = (Input::KeyCode)key;

                switch (action)
                {
                case GLFW_PRESS: NewEvent.InputState = Input::State::Begin; break;
                case GLFW_RELEASE: NewEvent.InputState = Input::State::End; break;
                }

                PointerInfo.App->PushInputEvent(NewEvent);
            });
	}

    Window::~Window()
    {
        glfwDestroyWindow(aWindow);
        glfwTerminate();
    }

    void Window::SetVsyncEnabled(bool Enabled)
    {
        if (Enabled)
        {
            Debug::Log("Vertical sync: Enabled", "Window");
            glfwSwapInterval(1);
        }
        else
        {
            Debug::Log("Vertical sync: Disabled", "Window");
            glfwSwapInterval(0);
        }
    }

    void Window::Clear()
    {
        Renderer::Clear();
    }

    void Window::Update()
    {
        glfwSwapBuffers(aWindow);
        glfwPollEvents();
    }
    
}