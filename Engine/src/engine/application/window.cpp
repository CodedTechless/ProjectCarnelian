
#include "window.h"
#include "render/renderer.h"

#include <engine/watchdog/watchdog.h>

void error_callback(int error, const char* msg) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

namespace Techless 
{
	
    Window::Window(const char AppTitle[], glm::uvec2 aSize)
        : Size(aSize)
    {
        glfwSetErrorCallback(error_callback);
        
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