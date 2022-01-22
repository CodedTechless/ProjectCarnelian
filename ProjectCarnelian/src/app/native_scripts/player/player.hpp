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
		auto& Transform = GetComponent<TransformComponent>();
		auto& Sprite = AddComponent<SpriteComponent>();
		Sprite.SetSprite("Player");

		auto Scene = GetScene();
		{
			auto& Entity = Scene->CreateEntity();

			auto& a_Transform = Entity.AddComponent<TransformComponent>();
			a_Transform.SetParent(&Transform);
			a_Transform = glm::vec3(-64.f, 64.f, 0.f);

			auto& a_Sprite = Entity.AddComponent<SpriteComponent>();
			a_Sprite.SetSprite("Player");
			a_Sprite.SetRGBColour(Colour(255, 0, 0));
		}
	}

	constexpr float Approach(float Value, float Approach, float Amount) 
	{
		return (Value > Approach) ? std::max(Value -= Amount, Approach) : std::min(Value += Amount, Approach);
	}

	Input::Filter OnInputEvent(const InputEvent& inputEvent, const bool Processed)
	{
		if (inputEvent.KeyCode == Input::KeyCode::E)
		{
			auto& Transform = GetComponent<TransformComponent>();
			Transform += glm::vec2(1.f, 1.f);

			Debug::Log("I'm the cum donator! (" + std::to_string(Transform.GetGlobalScale().x) + ", " + std::to_string(Transform.GetGlobalScale().y) + ")");
		}

		return Input::Filter::Ignore;
	}

	void OnFixedUpdate(const float Delta) 
	{
		auto& Transform = GetComponent<TransformComponent>();

		bool KeyUp = Input::KeyDown(Input::KeyCode::W);
		bool KeyDown = Input::KeyDown(Input::KeyCode::S);
		bool KeyLeft = Input::KeyDown(Input::KeyCode::A);
		bool KeyRight = Input::KeyDown(Input::KeyCode::D);

		int Horizontal = KeyRight - KeyLeft;
		int Vertical = KeyDown - KeyUp;

		int HorizontalAbs = abs(Horizontal);
		int VerticalAbs = abs(Vertical);

		float Angle = atan2(VerticalAbs, HorizontalAbs);

		float WalkSpeedMaxH = cos(Angle) * WalkSpeedMax;
		float WalkSpeedMaxV = sin(Angle) * WalkSpeedMax;

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

		if (abs(Velocity.x) > 0.f || abs(Velocity.y) > 0.f)
			Transform += glm::vec3(Velocity, 0.f);

		
		auto LocalPosition = Transform.GetLocalPosition();
		auto GlobalPosition = Transform.GetGlobalPosition();
		//Debug::Log("(" + std::to_string(LocalPosition.x) + " " + std::to_string(LocalPosition.y) + " " + std::to_string(LocalPosition.z) + ") (" + std::to_string(GlobalPosition.x) + " " + std::to_string(GlobalPosition.y) + " " + std::to_string(GlobalPosition.z) + ")");
		
		Transform += 1.f;

	}

};