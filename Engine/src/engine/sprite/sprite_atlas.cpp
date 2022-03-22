
#include "sprite_atlas.h"

#include <engineincl.h>

#include <render/renderer.h>

#include <engine/application/watchdog/watchdog.h>
#include <engine/application/resource_loader.h>

#include <stb_image/stb_image.h>
#include <stb_rect_pack/stb_rect_pack.h>

namespace fs = std::filesystem;

namespace Techless
{
	SpriteCache SpriteAtlas::m_SpriteCache{};
	TextureAtlas SpriteAtlas::m_TextureAtlas{};

	Ptr<Texture> SpriteAtlas::MissingTexture = nullptr;
	Ptr<Sprite> SpriteAtlas::MissingSprite = nullptr;

	// Sprite Atlas

	struct TextureInfo
	{
		std::string Name = "";

		int Width = 0;
		int Height = 0;
		int BitsPerPixel = 0;

		stbrp_rect* Rect = nullptr;
		unsigned char* Buffer = nullptr;

		bool Packed = false;
	};

	unsigned char* AllocateBuffer(unsigned int TextureSize)
	{
		const unsigned int BlockSize = TextureSize * TextureSize * 4;

		unsigned char* Tex = new unsigned char[BlockSize];
		memset(Tex, 0, BlockSize);

		return Tex;
	}

	void SpriteAtlas::Init()
	{
		MissingTexture = CreatePtr<Texture>("srcassets/textures/missing_texture.png");
		MissingSprite = CreatePtr<Sprite>(MissingTexture);

		SpriteAtlas::Load("srcassets");
	}

	void SpriteAtlas::Load(const std::string& Directory)
	{
		Debug::Log("Loading sprites from: " + Directory, "SpriteAtlas");

		std::vector<TextureInfo> Textures;

		ResourceLoader::GetFiles(Resource::Texture,
			[&](const fs::directory_entry& File)
			{
				fs::path Path = File.path();
				std::string sPath = Path.generic_string();

				TextureInfo textureInfo;

				stbi_set_flip_vertically_on_load(1);
				textureInfo.Buffer = stbi_load(sPath.c_str(), &textureInfo.Width, &textureInfo.Height, &textureInfo.BitsPerPixel, 4);
				textureInfo.Name = Path.stem().string();

				Textures.push_back(textureInfo);
			}, Directory);
		
		int MaxTextureSize = Renderer::GetMaxTextureSize();

		bool Success = false;
		while (!Success)
		{
			auto* PageBuffer = AllocateBuffer(MaxTextureSize);
//			Debug::Log("Allocated a " + std::to_string(MaxTextureSize) + "x" + std::to_string(MaxTextureSize) + " texture page (" + std::to_string(MaxTextureSize * MaxTextureSize * 4) + " bytes)", "SpriteAtlas/" + Directory);

			size_t TextureCount = Textures.size();

			stbrp_context NewContext;
			stbrp_rect* Rects = new stbrp_rect[TextureCount];
			stbrp_node* Nodes = new stbrp_node[TextureCount];

			int i;
			for (i = 0; i < TextureCount; ++i)
			{
				TextureInfo& Tex = Textures[i];

				stbrp_rect Rect = { i, Tex.Width, Tex.Height };
				Rects[i] = Rect;
			
				Tex.Rect = &Rects[i];
			}

			stbrp_init_target(&NewContext, MaxTextureSize, MaxTextureSize, Nodes, TextureCount);
			Success = stbrp_pack_rects(&NewContext, Rects, TextureCount);

			// if there's already an entry in the atlas, delete it so we can reload all of the related textures.
			if (m_TextureAtlas.find(Directory) != m_TextureAtlas.end())
			{
				m_TextureAtlas[Directory].clear();
			}
			else
			{
				m_TextureAtlas[Directory] = {};
			}

			Ptr<Texture> newTexture = CreatePtr<Texture>(glm::i32vec2(MaxTextureSize, MaxTextureSize), 4);
			m_TextureAtlas[Directory].push_back(newTexture);

			for (TextureInfo& textureInfo : Textures)
			{
				stbrp_rect& Rect = *textureInfo.Rect;
				
				if (Rect.was_packed)
				{
					for (int iY = 0; iY < Rect.h; ++iY)
					{
						for (int iX = 0; iX < Rect.w; ++iX)
						{
							int From = (iY * Rect.w * 4) + (iX * 4);
							int To = ((Rect.y + iY) * MaxTextureSize * 4) + ((Rect.x + iX) * 4);

							for (int Channel = 0; Channel < 4; ++Channel)
							{
								PageBuffer[To + Channel] = textureInfo.Buffer[From + Channel];
							}
						}
					}

					Vector2 TopLeft = { Rect.x, Rect.y };
					Vector2 BottomRight = { (Rect.x + Rect.w), (Rect.y + Rect.h) };

					// if this sprite is already cached, then we just have to reload it.
					if (m_SpriteCache.find(textureInfo.Name) != m_SpriteCache.end())
					{
						auto CachedSprite = m_SpriteCache[textureInfo.Name];
						CachedSprite->SetTexture(newTexture);
						CachedSprite->SetBounds(TopLeft, BottomRight);
					}
					else
					// if it's not cached, then create a new sprite.
					{
						m_SpriteCache[textureInfo.Name] = CreatePtr<Sprite>(newTexture, TopLeft, BottomRight, textureInfo.Name);
					}

					stbi_image_free(textureInfo.Buffer);
					textureInfo.Packed = true;
				}
			}

			newTexture->Push(PageBuffer);
			delete[] PageBuffer;
		}
	}

	Ptr<Sprite> SpriteAtlas::Get(const std::string& Name)
	{
		if (Has(Name))
			return m_SpriteCache[Name];
		
		return MissingSprite;
	}

	bool SpriteAtlas::Has(const std::string& Name)
	{
		return m_SpriteCache.find(Name) != m_SpriteCache.end();
	}
}