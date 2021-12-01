
#include "sprite_atlas.h"
#include <Engine.h>

namespace Techless
{
	JSON SpriteAtlas::Atlas = {};
	std::unordered_map<std::string,TextureRef> SpriteAtlas::TextureSheets = {};
	std::unordered_map<std::string, std::shared_ptr<Sprite>> SpriteAtlas::SpriteCache = {};

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

	Sprite::~Sprite()
	{
		//Debug::Log("Sprite destroyed");
	}


	// Sprite Atlas

	void SpriteAtlas::Init()
	{
		std::ifstream SpriteDataFile("assets/texdict.json");
		SpriteDataFile >> Atlas;
	}

	std::shared_ptr<Sprite> SpriteAtlas::GetSprite(const std::string& Name)
	{
		if (SpriteCache.find(Name) != SpriteCache.end())
			return SpriteCache[Name];

		// LOADING A SPRITE IS REALLY FUCKING SLOW!!!!! LIKE, DON'T DO THIS VERY OFTEN, DUDE!!!

		if (Atlas["sprites"].find(Name) == Atlas["sprites"].end())
			return nullptr;

		// Get the texture sheet that the sprite is a part of.
		auto SpriteData = Atlas["sprites"][Name];
		auto TextureSheetID = SpriteData["texture_sheet_name"].get<std::string>();
		auto Sheet = GetTextureSheet(TextureSheetID);

		if (!Sheet)
			Sheet = LoadTextureSheet(TextureSheetID, Atlas["texture_sheets"][TextureSheetID]);
		
		// Get the texture coordinates.
		auto TopLeftData = SpriteData["top_left"];
		auto BottomRightData = SpriteData["bottom_right"];

		glm::vec2 TopLeft = { TopLeftData["x"].get<unsigned int>() , TopLeftData["y"].get<unsigned int>() };
		glm::vec2 BottomRight = { BottomRightData["x"].get<unsigned int>() , BottomRightData["y"].get<unsigned int>() };

		auto NewSprite = std::make_shared<Sprite>(Sheet, TopLeft, BottomRight);
		SpriteCache[Name] = NewSprite;
		
		return NewSprite;
	}

	std::shared_ptr<Texture> SpriteAtlas::GetTextureSheet(const std::string& Name)
	{
		if (TextureSheets.find(Name) != TextureSheets.end())
			return TextureSheets[Name].Tex;

		return nullptr;
	}

	std::shared_ptr<Texture> SpriteAtlas::LoadTextureSheet(const std::string& Name, const std::string& TexturePath)
	{
		std::shared_ptr<Texture> Tex = std::make_shared<Texture>(TexturePath);
		TextureSheets[Name] = { Tex, 0 };

		return Tex;
	}

	void SpriteAtlas::FreeTextureSheet(const std::string& Name)
	{
		TextureSheets.erase(Name);
	}
}