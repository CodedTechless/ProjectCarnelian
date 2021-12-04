#pragma once

#include <engineincl.h>

#include <engine/input/input.h>

namespace Techless
{

	struct InputEvent
	{
		Input::Type InputType = Input::Type::None;
		Input::State InputState = Input::State::None;

		Input::KeyCode KeyCode = Input::KeyCode::None;
		Input::MouseCode MouseCode = Input::MouseCode::None;

		glm::vec3 Position = {};
		glm::vec3 Delta = {};
	};

	struct WindowEvent
	{

	};

}