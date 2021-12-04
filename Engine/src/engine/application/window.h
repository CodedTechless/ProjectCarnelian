#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace Techless
{
	class Window
	{
	public:
		Window(const char AppTitle[], glm::uvec2 Size);

		void Clear();
		void SetVsyncEnabled(bool Enabled);
		
		void Update();

		glm::uvec2 Size;
		inline GLFWwindow* GetContext() const { return aWindow; };
	private:
		GLFWwindow* aWindow;
		
	};
}