
#include "sprite_atlas.h"

#include <Engine.h>
#include <engineincl.h>

#include <stb_image/stb_image.h>
#include <stb_rect_pack/stb_rect_pack.h>

namespace fs = std::filesystem;

namespace Techless
{

	std::unordered_map<std::string, std::shared_ptr<Sprite>> SpriteAtlas::SpriteCache{};
	std::array<Ptr<Texture>, 8> SpriteAtlas::TexturePages{};

	unsigned int SpriteAtlas::TexturePageIndex = 0;

	// Sprite Atlas

	struct TextureInfo
	{
		std::string Name = "";

		int Width = 0;
		int Height = 0;
		int BitsPerPixel = 0;

		unsigned char* Buffer;
	};

	unsigned char* AllocateBuffer(unsigned int TextureSize)
	{
		const unsigned int BlockSize = TextureSize * TextureSize * 4;

		unsigned char* Tex = new unsigned char[BlockSize];
		memset(Tex, 255, BlockSize);

		Debug::Log("Allocated a " + std::to_string(TextureSize) + "x" + std::to_string(TextureSize) + " texture page", "SpriteAtlas");

		return Tex;
	}

	void Read(std::vector<TextureInfo>& TextureBuffers, const std::string& Path)
	{
		for (const auto& File : fs::directory_iterator(Path))
		{
			auto FsPath = File.path();
			auto Path = FsPath.generic_string();

			if (File.is_directory())
			{
				Read(TextureBuffers, Path);
			} 
			else if (File.is_regular_file() && Path.substr(Path.size() - 4) == ".png")
			{
				TextureInfo textureInfo;

				stbi_set_flip_vertically_on_load(1);
				textureInfo.Buffer = stbi_load(Path.c_str(), &textureInfo.Width, &textureInfo.Height, &textureInfo.BitsPerPixel, 4);
				textureInfo.Name = FsPath.stem().string();

				TextureBuffers.push_back(textureInfo);

				Debug::Log("Loaded " + textureInfo.Name, "SpriteAtlas");
			}
		}
	}

	void SpriteAtlas::Init()
	{
		Debug::Log("Loading textures...", "SpriteAtlas");

		std::vector<TextureInfo> Textures;
		Read(Textures, "assets");
		
		auto MaxTextureSize = Renderer::GetMaxTextureSize();

		bool Success = false;
		while (!Success)
		{
			auto* PageBuffer = AllocateBuffer(MaxTextureSize);
			auto TextureCount = Textures.size();

			stbrp_context NewContext;
			stbrp_rect* Rects = new stbrp_rect[TextureCount];
			stbrp_node* Nodes = new stbrp_node[TextureCount];

			int i;
			for (i = 0; i < TextureCount; ++i)
			{
				TextureInfo& Tex = Textures[i];
				Rects[i] = { i, Tex.Width, Tex.Height };
			}

			stbrp_init_target(&NewContext, MaxTextureSize, MaxTextureSize, Nodes, TextureCount);
			Success = stbrp_pack_rects(&NewContext, Rects, TextureCount);

			Ptr<Texture> newTexture = CreatePtr<Texture>();
			TexturePages[TexturePageIndex++] = newTexture;

			for (i = 0; i < TextureCount; ++i)
			{
				auto& Rect = Rects[i];
				
				if (Rect.was_packed)
				{
					auto& textureInfo = Textures[i];

					for (int iY = 0; iY < Rect.w; ++iY)
					{
						for (int iX = 0; iX < Rect.h; ++iX)
						{
							int From = (iY * Rect.w * 4) + (iX * 4);
							int To = ((Rect.y + iY) * MaxTextureSize * 4) + ((Rect.x + iX) * 4);

							for (int Channel = 0; Channel < 4; ++Channel)
							{
								PageBuffer[To + Channel] = textureInfo.Buffer[From + Channel];
							}
						}
					}

					Ptr<Sprite> newSprite = CreatePtr<Sprite>(newTexture, glm::vec2(Rect.x / MaxTextureSize, Rect.y / MaxTextureSize), glm::vec2((Rect.x + Rect.w) / MaxTextureSize, (Rect.y + Rect.h) / MaxTextureSize ));
					SpriteCache[textureInfo.Name] = newSprite;

					stbi_image_free(Textures[i].Buffer);
					Textures.erase(Textures.begin() + i);
					
					i--; TextureCount--;
				}
			}

			newTexture->Push(PageBuffer);
		}
	}

	std::shared_ptr<Sprite> SpriteAtlas::GetSprite(const std::string& Name)
	{
		if (SpriteCache.find(Name) != SpriteCache.end())
			return SpriteCache[Name];
		
		return nullptr;
	}
}