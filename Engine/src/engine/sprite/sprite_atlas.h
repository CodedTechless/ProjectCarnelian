#pragma once

#include <engineincl.h>

#include "sprite.h"
#include "render/texture/texture.h"

namespace Techless
{

	class SpriteAtlas
	{
	public:
		static void Init();

		static std::shared_ptr<Sprite> GetSprite(const std::string& Name);

		static std::shared_ptr<Texture> GetTexturePage(int Index) { return TexturePages[Index]; };
		static std::unordered_map<std::string, Ptr<Sprite>>& GetSpriteCache() { return SpriteCache; };

	private:
		static std::unordered_map<std::string, Ptr<Sprite>> SpriteCache;
		static std::array<Ptr<Texture>, 8> TexturePages;

		static Ptr<Sprite> MissingTexture;

		static unsigned int TexturePageIndex;
	};
}