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

		inline GLFWwindow* GetContext() const { return aWindow; };

		bool IsFocused = true;
		glm::uvec2 Size;
	private:
		GLFWwindow* aWindow;
	};
}