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

		void SetBounds();
		void SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight);

		inline Bounds GetBounds() const { return { TopLeft, BottomRight }; };
		inline Bounds GetAbsoluteBounds() const { return { AbsoluteTopLeft, AbsoluteBottomRight }; };
		inline std::shared_ptr<Texture> GetTexture() const { return BaseTexture; };
		inline glm::vec2 GetSize() const { return BaseTexture->GetDimensions(); };

	private:
		void UpdateAbsoluteBounds();

		std::shared_ptr<Texture> BaseTexture;

		glm::vec2 TopLeft;
		glm::vec2 BottomRight;

		glm::vec2 AbsoluteTopLeft;
		glm::vec2 AbsoluteBottomRight;
	};
}