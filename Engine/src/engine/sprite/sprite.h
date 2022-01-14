#pragma once

#include <render/texture/texture.h>

#include <glm/glm.hpp>

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
		Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft = { 0, 0 }, const glm::vec2& bottomRight = { INFINITY, INFINITY }, const std::string& Name = "Sprite");

		void SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight);

		inline Bounds GetBounds() const { return { TopLeft, BottomRight }; };
		inline Bounds GetAbsoluteBounds() const { return { AbsoluteTopLeft, AbsoluteBottomRight }; };
		
		inline std::shared_ptr<Texture> GetTexture() const { return BaseTexture; };
		inline glm::vec2 GetSize() const { return Size; };

		inline std::string GetName() const { return Name; };

	private:
		std::shared_ptr<Texture> BaseTexture;

		glm::vec2 Size = { 0.f, 0.f };

		glm::vec2 TopLeft;
		glm::vec2 BottomRight;

		glm::vec2 AbsoluteTopLeft;
		glm::vec2 AbsoluteBottomRight;

		std::string Name;
	};
}