#pragma once

#include <glm/glm.hpp>
#include <engineincl.h>

namespace Techless {
	class Texture {
	public:
		Texture(const glm::i32vec2& dimensions, int bitsPerPixel);
		Texture(const std::string& Path);
		~Texture();

		void Load(const std::string& Path);
		void Push(unsigned char* Buffer);

		void Bind(unsigned int Slot = 0) const;
		void Unbind() const;

		inline glm::i32vec2 GetDimensions() const { return Dimensions; };
		inline std::string GetFilePath() const { return FilePath; };
		inline unsigned int GetRendererID() const { return RendererID; };
	private:
		void Allocate();
	private:
		unsigned int RendererID = 0;
		std::string FilePath = "";
		unsigned char* LocalBuffer = nullptr;

		// matt-eta
		glm::i32vec2 Dimensions = { 0, 0 };
		int BitsPerPixel = 0;
	};
}