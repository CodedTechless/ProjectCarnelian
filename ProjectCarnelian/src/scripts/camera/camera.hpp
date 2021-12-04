#pragma once

#include <Engine.h>

using namespace Techless;

class Camera : public ScriptableEntity
{
public:

	float CameraSpeed = 10.f;
	float CameraDiffMultiplier = 0.4f;

	glm::vec2 PositionTo = {};

	void OnCreate()
	{
		PositionTo = GetComponent<TransformComponent>().Position;

		Debug::Log("Hello!!");
	}


	void OnUpdate(const float Delta)
	{
		auto& Transform = GetComponent<TransformComponent>();

		Transform.Position += (PositionTo - Transform.Position) * 0.2f * Delta;
	}

	void OnFixedUpdate(const float Delta)
	{
		if (Input::KeyDown(Keyboard::KeyCodes::A))
			PositionTo -= glm::vec2(CameraSpeed * Delta, 0);
		if (Input::KeyDown(Keyboard::KeyCodes::D))
			PositionTo += glm::vec2(CameraSpeed * Delta, 0);
		if (Input::KeyDown(Keyboard::KeyCodes::W))
			PositionTo -= glm::vec2(0, CameraSpeed * Delta);
		if (Input::KeyDown(Keyboard::KeyCodes::S))
			PositionTo += glm::vec2(0, CameraSpeed * Delta);
	}
};