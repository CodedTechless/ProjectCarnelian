#pragma once

#include <render/texture/texture.h>
#include <engineincl.h>

#include "sprite.h"

namespace Techless
{

	class SpriteAtlas
	{
	public:
		static void Init();

		static Ptr<Sprite> Get(const std::string& Name);

		static Ptr<Texture> GetTexturePage(int Index) { return TexturePages[Index]; };
		static std::unordered_map<std::string, Ptr<Sprite>>& GetSpriteCache() { return SpriteCache; };

		static Ptr<Texture> GetMissingTexture() { return MissingTexture; };
		static Ptr<Sprite> GetMissingSprite() { return MissingSprite; };

	private:
		static std::unordered_map<std::string, Ptr<Sprite>> SpriteCache;
		static std::array<Ptr<Texture>, 8> TexturePages;
		
		static Ptr<Texture> MissingTexture;
		static Ptr<Sprite> MissingSprite;

		static unsigned int TexturePageIndex;
	};
}