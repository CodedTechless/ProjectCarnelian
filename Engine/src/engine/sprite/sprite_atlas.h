#pragma once

#include <engineincl.h>

#include "glm/glm.hpp"
#include "json/json.hpp"

using JSON = nlohmann::json;

#include "render/texture/texture.h"

namespace Techless
{

	struct Bounds
	{
		glm::vec2 TopLeft;
		glm::vec2 BottomRight;
	};

	struct TextureRef
	{
		std::shared_ptr<Texture> Tex;
		uint16_t References = 0;
	};

	class Sprite
	{
	public:
		Sprite(std::shared_ptr<Texture> Tex);
		Sprite(std::shared_ptr<Texture> Tex, const glm::vec2& topLeft, const glm::vec2& bottomRight);
		~Sprite();

		void SetBounds(glm::vec2 topLeft, glm::vec2 bottomRight) { TopLeft = topLeft; bottomRight = bottomRight; };

		inline Bounds GetBounds() const { return { TopLeft, BottomRight }; };
		inline std::shared_ptr<Texture> GetTexture() const { return BaseTexture; };
	private:
		std::shared_ptr<Texture> BaseTexture;

		glm::vec2 TopLeft;
		glm::vec2 BottomRight;
	};

	class SpriteAtlas
	{
	public:
		static void Init();

		static std::shared_ptr<Sprite> GetSprite(const std::string& Name);

		static void FreeTextureSheet(const std::string& Name);
		static std::shared_ptr<Texture> GetTextureSheet(const std::string& Name);
		static std::shared_ptr<Texture> LoadTextureSheet(const std::string& Name, const std::string& TexturePath);
		
		static inline uint16_t GetTextureReferenceCount(const std::string& FileName)   { return TextureSheets[FileName].References; };
		static inline JSON& GetAtlas() { return Atlas; };

	private:
		static JSON Atlas;
		static std::unordered_map<std::string, std::shared_ptr<Sprite>> SpriteCache;
		static std::unordered_map<std::string, TextureRef> TextureSheets;
	};
}