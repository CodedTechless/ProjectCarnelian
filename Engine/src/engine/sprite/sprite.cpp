
#include "sprite.h"


namespace Techless
{
	Sprite::Sprite(std::shared_ptr<Texture> Tex)
		: BaseTexture(Tex)
	{
		SetBounds();
	}

	Sprite::Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft, const glm::vec2& bottomRight)
		: BaseTexture(Tex)
	{
		SetBounds(topLeft, bottomRight);
	};

	void Sprite::SetBounds()
	{
		auto TextureSize = BaseTexture->GetDimensions();

		TopLeft = { 0, 0 };
		BottomRight = { TextureSize.x, TextureSize.y };

		AbsoluteTopLeft = TopLeft / (glm::vec2)TextureSize;
		AbsoluteBottomRight = BottomRight / (glm::vec2)TextureSize;
	}

	void Sprite::SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight)
	{
		TopLeft = topLeft;
		BottomRight = bottomRight;
		
		UpdateAbsoluteBounds();
	}

	void Sprite::UpdateAbsoluteBounds()
	{
		auto TextureSize = BaseTexture->GetDimensions();

		AbsoluteTopLeft = TopLeft / (glm::vec2)TextureSize;
		AbsoluteBottomRight = BottomRight / (glm::vec2)TextureSize;
	}
}