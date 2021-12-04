#pragma once

#include <Engine.h>

using namespace Techless;

class Player : public ScriptableEntity
{
public:


	void OnCreate()
	{
		auto& Transform = AddComponent<TransformComponent>();
		Transform.Scale = { 32.f, 32.f };

		auto& Sprite = AddComponent<SpriteComponent>();
		Sprite.aSprite = SpriteAtlas::GetSprite("Player");
		Sprite.Offset = glm::vec3(0.f, -16.f, 0.f);
	}

};