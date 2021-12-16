
#include <GL/glew.h>
#include <stb_image/stb_image.h>
#include <stb_rect_pack/stb_rect_pack.h>

#include "texture.h"

namespace Techless {
	Texture::Texture(const glm::i32vec2& dimensions, int bitsPerPixel)
		: Dimensions(dimensions), BitsPerPixel(bitsPerPixel)
	{
		Allocate();
	};

	Texture::Texture(const std::string& filePath)
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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Dimensions.x, Dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Push(unsigned char* Buffer)
	{
		LocalBuffer = Buffer;

		glBindTexture(GL_TEXTURE_2D, RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Dimensions.x, Dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Load(const std::string& FilePath) 
	{
		stbi_set_flip_vertically_on_load(1);
		LocalBuffer = stbi_load(FilePath.c_str(), &Dimensions.x, &Dimensions.y, &BitsPerPixel, 4);

		glBindTexture(GL_TEXTURE_2D, RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Dimensions.x, Dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (LocalBuffer) {
			stbi_image_free(LocalBuffer);
		}
	}

//	void Texture::Pack()
}