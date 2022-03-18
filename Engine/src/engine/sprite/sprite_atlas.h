#pragma once

#include <render/texture/texture.h>

#include "sprite.h"

namespace Techless
{
	using TextureAtlas = std::unordered_map<std::string, std::vector<Ptr<Texture>>>;
	using SpriteCache = std::unordered_map<std::string, Ptr<Sprite>>;

	class SpriteAtlas
	{
	public:
		static void Init();

		static void Load(const std::string& Directory);

		static Ptr<Sprite> Get(const std::string& Name);
		static bool Has(const std::string& Name);

	public:
		static Ptr<Texture> GetMissingTexture() { return MissingTexture; };
		static Ptr<Sprite> GetMissingSprite() { return MissingSprite; };

	private:
		static SpriteCache m_SpriteCache;
		static TextureAtlas m_TextureAtlas;
		
		static Ptr<Texture> MissingTexture;
		static Ptr<Sprite> MissingSprite;

	};
}