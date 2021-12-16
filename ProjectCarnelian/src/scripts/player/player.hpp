#pragma once

#include <Engine.h>

using namespace Techless;

class Player : public ScriptableEntity
{
public:

	float WalkSpeedMax = 3.f;
	float WalkAccel = 0.75f;
	float WalkFric = 1.25f;

	glm::vec2 Velocity;

	void OnCreate()
	{
		auto& Transform = AddComponent<TransformComponent>();

		auto& Sprite = AddComponent<SpriteComponent>();
		Sprite.SetSprite("Player");
		Sprite.Offset = glm::vec3(0.f, -16.f, 0.f);

		auto& Scene = GetScene();
		auto& Entity = Scene.CreateEntity();
		Entity.AddComponent<TransformComponent>();
	}

	constexpr float Approach(float Value, float Approach, float Amount) {
		return (Value > Approach) ? std::max(Value -= Amount, Approach) : std::min(Value += Amount, Approach);
	}

	void OnFixedUpdate(const float Delta) {
		auto& Transform = GetComponent<TransformComponent>();

		bool KeyUp = Input::KeyDown(Input::KeyCode::W);
		bool KeyDown = Input::KeyDown(Input::KeyCode::S);
		bool KeyLeft = Input::KeyDown(Input::KeyCode::A);
		bool KeyRight = Input::KeyDown(Input::KeyCode::D);

		int Horizontal = KeyRight - KeyLeft;
		int Vertical = KeyDown - KeyUp;

		int HorizontalAbs = std::abs(Horizontal);
		int VerticalAbs = std::abs(Vertical);

		float Angle = std::atan2(VerticalAbs, HorizontalAbs);

		float WalkSpeedMaxH = std::cos(Angle) * WalkSpeedMax;
		float WalkSpeedMaxV = std::sin(Angle) * WalkSpeedMax;

		Velocity.x += WalkAccel * Horizontal;
		Velocity.y += WalkAccel * Vertical;

		if (Horizontal == 0) {
			Velocity.x = Approach(Velocity.x, 0, WalkFric);
		}
		else if (std::abs(Velocity.x) > std::abs(WalkSpeedMaxH)) {
			Velocity.x = Approach(Velocity.x, WalkSpeedMaxH * sign(Velocity.x), WalkFric);
		};

		if (Vertical == 0) {
			Velocity.y = Approach(Velocity.y, 0, WalkFric);
		}
		else if (std::abs(Velocity.y) > std::abs(WalkSpeedMaxV)) {
			Velocity.y = Approach(Velocity.y, WalkSpeedMaxV * sign(Velocity.y), WalkFric);
		}

		Transform += glm::vec3(Velocity, 0.f);

		/*
		auto LocalPosition = Transform.GetLocalPosition();
		auto GlobalPosition = Transform.GetGlobalPosition();
		Debug::Log("(" + std::to_string(LocalPosition.x) + " " + std::to_string(LocalPosition.y) + " " + std::to_string(LocalPosition.z) + ") (" + std::to_string(GlobalPosition.x) + " " + std::to_string(GlobalPosition.y) + " " + std::to_string(GlobalPosition.z) + ")");
		*/
	}

};