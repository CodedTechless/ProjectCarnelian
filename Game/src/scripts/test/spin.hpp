#pragma once

#include <Engine.h>

using namespace Techless;

class Spin : public ScriptableEntity
{
public:

	float Pos = 0;

	void OnUpdate(const float Delta)
	{
		auto& Transform = GetComponent<TransformComponent>();

		Transform.Angle = Transform.Angle + 1.f * Delta;

		auto& Sprite = GetComponent<SpriteComponent>();
		Sprite.Colour = glm::vec3(std::sin(Pos) + 1, std::sin(Pos + 2) + 1, std::sin(Pos + 3) + 1);

		Pos += 0.01f * Delta;
	}
};