#pragma once

#include <glm/glm.hpp>
#include <engineincl.h>

namespace Techless {
	class Texture {
	public:
		Texture();
		Texture(const std::string& Path);
		~Texture();

		void Load(const std::string& Path);
		void Push(const void* Buffer);

		void Bind(unsigned int Slot = 0) const;
		void Unbind() const;

		inline glm::vec2 GetDimensions() const { return glm::vec2(Width, Height); };
		inline std::string GetFilePath() const { return FilePath; };
	private:
		void Allocate();
	private:
		unsigned int RendererID;
		std::string FilePath;
		unsigned char* LocalBuffer;

		// matt-eta
		int Width;
		int Height;
		int BitsPerPixel;
	};
}