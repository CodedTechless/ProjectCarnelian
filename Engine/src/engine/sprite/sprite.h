#pragma once


#include "glm/glm.hpp"

#include <render/texture/texture.h>

namespace Techless
{
	struct Bounds
	{
		glm::vec2 TopLeft;
		glm::vec2 BottomRight;
	};

	class Sprite
	{
	public:
		Sprite(std::shared_ptr<Texture> Tex);
		Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft, const glm::vec2& bottomRight);

		void SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight) { TopLeft = topLeft; bottomRight = bottomRight; };

		inline Bounds GetBounds() const { return { TopLeft, BottomRight }; };
		inline std::shared_ptr<Texture> GetTexture() const { return BaseTexture; };
	private:
		std::shared_ptr<Texture> BaseTexture;

		glm::vec2 TopLeft;
		glm::vec2 BottomRight;
	};
}