
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
        Application& App;
        Window& ActiveWindow;
    };

    Window::Window(const char AppTitle[], glm::uvec2 aSize)
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
        UserPointerInfo userPointerInfo = { Application::GetActiveApplication(), *this };

        glfwSetWindowUserPointer(aWindow, (void*)&userPointerInfo);

        glfwSetWindowCloseCallback(aWindow,
            [](GLFWwindow* window)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);
                PointerInfo.App.End();
            });

        glfwSetWindowSizeCallback(aWindow,
            [](GLFWwindow* window, int Width, int Height)
            {
                UserPointerInfo& PointerInfo = *(UserPointerInfo*)glfwGetWindowUserPointer(window);
                PointerInfo.ActiveWindow.Size = glm::uvec2(Width, Height);

                //Debug::Log("Window Resize " + std::to_string(Width) + "x" + std::to_string(Height), "GLFW");
            });

        glfwSetFramebufferSizeCallback(aWindow,
            [](GLFWwindow* window, int Width, int Height)
            {
                glViewport(0, 0, Width, Height);

                //Debug::Log("Framebuffer Resize " + std::to_string(Width) + "x" + std::to_string(Height), "GLFW");
            });

        glfwSetScrollCallback(aWindow,
            [](GLFWwindow* window, double xOffset, double yOffset)
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                Input::OnScrollWheelInput(yOffset);
            });

        glfwSetCursorPosCallback(aWindow,
            [](GLFWwindow* window, double xPos, double yPos)
            {
                Application& App = *(Application*)glfwGetWindowUserPointer(window);
                Input::OnMousePositionInput({ xPos, yPos });
            });
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
        Renderer::RenderImGuiElements();

        glfwSwapBuffers(aWindow);
        glfwPollEvents();
    }
    
}