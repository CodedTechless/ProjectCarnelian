
#include "sprite.h"


namespace Techless
{

	Sprite::Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft, const glm::vec2& bottomRight, const std::string& name)
		: BaseTexture(Tex), Name(name)
	{
		SetBounds(topLeft, bottomRight);
	};

	void Sprite::SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight)
	{
		auto TextureSize = BaseTexture->GetDimensions();

		TopLeft = topLeft;
		BottomRight = bottomRight.x >= INFINITY || bottomRight.y >= INFINITY ? (glm::vec2)TextureSize : bottomRight;

		Size = BottomRight - TopLeft;
		
		AbsoluteTopLeft = TopLeft / (glm::vec2)TextureSize;
		AbsoluteBottomRight = BottomRight / (glm::vec2)TextureSize;
	}
}