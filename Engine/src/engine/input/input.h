#pragma once



#include "glm/glm.hpp"
#include "key_codes.h"
#include "mouse_codes.h"

namespace Techless
{

	class Input
	{
	public:

		static bool KeyDown(const Keyboard::KeyCodes Key);
		static bool MouseButtonDown(const Mouse::MouseCodes Button);

		static glm::vec2 GetMousePosition();
	};

	struct InputObject
	{
		glm::vec2 MousePos;
		float ScrollWheelPosition;
	};
}