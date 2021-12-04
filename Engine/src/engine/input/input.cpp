
#include "input.h"

#include "engine/application/application.h"
#include "GLFW/glfw3.h"

namespace Techless
{
	bool Input::KeyDown(const Input::KeyCode Key)
	{
		auto Window = Application::GetActiveApplication().GetActiveWindow()->GetContext();
		auto State = glfwGetKey(Window, (int)Key);

		return State == GLFW_PRESS || State == GLFW_REPEAT;
	}
	
	bool Input::MouseButtonDown(const Input::MouseCode Button)
	{
		auto Window = Application::GetActiveApplication().GetActiveWindow()->GetContext();
		auto State = glfwGetMouseButton(Window, (int)Button);

		return State == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition() 
	{
		auto Window = Application::GetActiveApplication().GetActiveWindow();
		auto Context = Window->GetContext();
		auto Size = Window->Size;

		double X, Y;
		glfwGetCursorPos(Context, &X, &Y);

		return { std::min((float)Size.x, std::max(0.f, (float)X)) , std::min((float)Size.y, std::max(0.f, (float)Y)) };
	}
}
