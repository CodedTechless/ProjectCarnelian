
#include <GL/glew.h>
#include <stb_image/stb_image.h>
#include <stb_rect_pack/stb_rect_pack.h>

#include "texture.h"

namespace Techless {
	Texture::Texture()
		: RendererID(0), FilePath(""), LocalBuffer(nullptr), Width(0), Height(0), BitsPerPixel(0) 
	{
		Allocate();
	};

	Texture::Texture(const std::string& FilePath) 
		: RendererID(0), FilePath(FilePath), LocalBuffer(nullptr), Width(0), Height(0), BitsPerPixel(0)
	{
		Allocate();
		Load(FilePath);
	}

	Texture::~Texture() 
	{
		glDeleteTextures(1, &RendererID);
	}

	void Texture::Bind(unsigned int Slot) const 
	{
		glActiveTexture(GL_TEXTURE0 + Slot);
		glBindTexture(GL_TEXTURE_2D, RendererID);
	}

	void Texture::Unbind() const 
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Allocate() 
	{
		glGenTextures(1, &RendererID);
		glBindTexture(GL_TEXTURE_2D, RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // sets the filter option when the texture is smaller than the actual image
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // same as above but for bigger textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // horizontal clamping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // horizontal clamping

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Push(unsigned char* Buffer, int TextureWidth, int TextureHeight, int BitsPerPixel)
	{
		LocalBuffer = Buffer;
		
		Width = TextureWidth;
		Height = TextureHeight;

		glBindTexture(GL_TEXTURE_2D, RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Load(const std::string& FilePath) 
	{
		stbi_set_flip_vertically_on_load(1);
		LocalBuffer = stbi_load(FilePath.c_str(), &Width, &Height, &BitsPerPixel, 4);

		glBindTexture(GL_TEXTURE_2D, RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (LocalBuffer) {
			stbi_image_free(LocalBuffer);
		}
	}

//	void Texture::Pack()
}