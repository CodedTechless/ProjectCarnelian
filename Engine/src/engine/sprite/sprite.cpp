
#include "sprite.h"


namespace Techless
{
	Sprite::Sprite(std::shared_ptr<Texture> Tex)
		: BaseTexture(Tex)
	{
		auto d = Tex->GetDimensions();

		TopLeft = { 0, 0 };
		BottomRight = { d.x, d.y };

		//Debug::Log("Created Sprite");
	}

	Sprite::Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft, const glm::vec2& bottomRight)
		: BaseTexture(Tex), TopLeft(topLeft), BottomRight(bottomRight)
	{
		//Debug::Log("Created Sprite");
	};
}