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

	private:
		static std::unordered_map<std::string, std::shared_ptr<Sprite>> SpriteCache;
		static std::array<Ptr<Texture>, 8> TexturePages;

		static unsigned int TexturePageIndex;
	};
}