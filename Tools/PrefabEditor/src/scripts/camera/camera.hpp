#pragma once

#include <Engine.h>

using namespace Techless;

namespace NativeScript
{
	namespace Core
	{

		class Camera : public ScriptableEntity
		{
		public:
			// Settings
			bool FreeCamera = true;
			TransformComponent* Subject = nullptr;

			bool AcceptingInput = true;

			// Movement
			float CameraSpeed = 1000.f;

			glm::vec3 PositionTo = {};

			// Zooming
			float ZoomLevel = 1.f;
			float ZoomLevelTo = 1.f;

			float ZoomSpeed = 0.1f;

			glm::vec2 ViewportSize = {};

			// Z-Plane
			float Near = -100.f;
			float Far = 100.f;

			void OnCreate()
			{
				auto& Transform = GetComponent<TransformComponent>();

				//Transform.SetEngineInterpolation(true);
				PositionTo = Transform.GetLocalPosition();
			}

			void OnUpdate(const float Delta)
			{
				auto& Transform = GetComponent<TransformComponent>();
				Transform.SetLocalPosition(Transform.GetLocalPosition() + (PositionTo - Transform.GetLocalPosition()) * 0.15f);

				auto WindowSize = Application::GetActiveApplication().GetActiveWindow()->Size;

				auto& CameraComp = GetComponent<CameraComponent>();
				CameraComp.SetOrthoSize(ViewportSize * ZoomLevel);

				ZoomLevel += ((ZoomLevelTo - ZoomLevel) * 0.3f);
			}

			void OnFixedUpdate(const float Delta)
			{
				if (FreeCamera)
				{
					int Horizontal = 0;
					int Vertical = 0;

					if (AcceptingInput)
					{
						Horizontal = (int)Input::KeyDown(Input::KeyCode::D) - (int)Input::KeyDown(Input::KeyCode::A);
						Vertical = (int)Input::KeyDown(Input::KeyCode::S) - (int)Input::KeyDown(Input::KeyCode::W);
					}

					PositionTo += glm::vec3(Horizontal, Vertical, 0) * CameraSpeed * ZoomLevel * Delta;

					//Debug::Log(std::to_string(PositionTo.x) + " " + std::to_string(PositionTo.y) + " " + std::to_string(PositionTo.z));
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
				ViewportSize = windowEvent.Size;

				auto& CameraComp = GetComponent<CameraComponent>();
				CameraComp.SetOrthoSize(ViewportSize * ZoomLevel);
			}
		};

	}
}