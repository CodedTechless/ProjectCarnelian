#pragma once

#include <Engine.h>

using namespace Techless;

class TextureSheetDrawer : public ScriptableEntity
{
public:

	glm::vec2 Size = { 256.f, 256.f};
	glm::vec3 Position = { -300.f, 0.f, 0.f };

	glm::vec2 TexCoords[4] = { {0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };

	void OnDraw(const float Delta)
	{
		auto TexturePage = SpriteAtlas::GetTexturePage(0);

		Renderer::DrawQuad(TexturePage, TexCoords, Position, Size);

		auto& SpriteCache = SpriteAtlas::GetSpriteCache();

		for (auto SpritePair : SpriteCache)
		{
			auto [Name, Sprite] = SpritePair;

			auto Bounds = Sprite->GetBounds();

			Bounds.TopLeft -= Size / 2.f;
			Bounds.BottomRight -= Size / 2.f;

			auto SpriteSize = (Bounds.BottomRight - Bounds.TopLeft);
			Renderer::DrawBlankQuad(Position + glm::vec3(Bounds.TopLeft + SpriteSize / 2.f, 50), SpriteSize, glm::vec4(1.f, 0.f, 0.f, 0.5f));

			Renderer::DrawBlankQuad(Position + glm::vec3(Bounds.TopLeft, 60), glm::vec2(8, 8), glm::vec4(1.f, 0.f, 0.f, 1.f));
			Renderer::DrawBlankQuad(Position + glm::vec3(Bounds.BottomRight, 60), glm::vec2(8, 8), glm::vec4(0.f, 0.f, 1.f, 1.f));
		}
	}
};