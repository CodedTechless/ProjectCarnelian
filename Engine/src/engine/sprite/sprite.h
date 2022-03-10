#pragma once

#include <render/texture/texture.h>
#include <engineincl.h>

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
		Sprite() = default;
		Sprite(Ptr<Texture> Tex, const glm::vec2& topLeft = { 0, 0 }, const glm::vec2& bottomRight = { INFINITY, INFINITY }, const std::string& Name = "Sprite");

		void SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight);

		inline Bounds GetBounds() const { return { TopLeft, BottomRight }; };
		inline Bounds GetAbsoluteBounds() const { return { AbsoluteTopLeft, AbsoluteBottomRight }; };
		
		inline Ptr<Texture> GetTexture() const { return BaseTexture; };
		inline glm::vec2 GetSize() const { return Size; };

		inline std::string GetName() const { return Name; };

	private:
		Ptr<Texture> BaseTexture;

		glm::vec2 Size = { 0.f, 0.f };

		glm::vec2 TopLeft = { 0.f, 0.f };
		glm::vec2 BottomRight = { INFINITY, INFINITY };

		glm::vec2 AbsoluteTopLeft = { 0.f, 0.f };
		glm::vec2 AbsoluteBottomRight = { 1.f, 1.f };

		std::string Name = "";
	};
}