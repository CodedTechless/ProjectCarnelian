#pragma once

#include <Engine.h>

using namespace Techless;

class Camera : public ScriptableEntity
{
public:
	// Settings
	bool FreeCamera = false;
	TransformComponent* Subject = nullptr;

	// Movement
	float CameraSpeed = 15.f;
	float CameraDiffMultiplier = 0.4f;

	glm::vec3 PositionTo = {};

	// Zooming
	float ZoomLevel = 1.f;
	float ZoomLevelTo = 1.f;
	
	float ZoomSpeed = 0.1f;

	// Z-Plane
	float Near = -100.f;
	float Far = 100.f;

	void OnCreate()
	{
		PositionTo = GetComponent<TransformComponent>().GetLocalPosition();
	}

	void OnUpdate(const float Delta)
	{
		auto& Transform = GetComponent<TransformComponent>();
		Transform += (PositionTo - Transform.GetLocalPosition()) * 0.2f * Delta;
	
		auto WindowSize = Application::GetActiveApplication().GetActiveWindow()->Size;

		auto& CameraComp = GetComponent<CameraComponent>();
		CameraComp.SetProjection((glm::vec2)WindowSize * ZoomLevel, Near, Far);

		ZoomLevel += (ZoomLevelTo - ZoomLevel) * 0.3f * Delta;
	}

	void OnFixedUpdate(const float Delta)
	{
		if (FreeCamera)
		{
			auto Horizontal = (int)Input::KeyDown(Input::KeyCode::D) - (int)Input::KeyDown(Input::KeyCode::A);
			auto Vertical = (int)Input::KeyDown(Input::KeyCode::S) - (int)Input::KeyDown(Input::KeyCode::W);

			PositionTo += glm::vec3(Horizontal, Vertical, 0) * CameraSpeed * ZoomLevel * Delta;
		}
		else if (Subject)
		{
			PositionTo = Subject->GetLocalPosition();
		}
	}

	Input::Filter OnInputEvent(const InputEvent& inputEvent, bool Processed)
	{
		if (inputEvent.InputType == Input::Type::Scrolling)
		{
			ZoomLevelTo += -inputEvent.Delta.y * ZoomSpeed * ZoomLevelTo;
		
			return Input::Filter::Continue;
		}

		return Input::Filter::Ignore;
	}

	void OnWindowEvent(const WindowEvent& windowEvent)
	{
		auto& CameraComp = GetComponent<CameraComponent>();
		CameraComp.SetProjection(windowEvent.Size * ZoomLevel, Near, Far);
	}
};