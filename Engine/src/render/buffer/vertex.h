#pragma once

#include <GL/glew.h>
#include <engineincl.h>

namespace Techless {
	enum class LayoutType {
		Float = 5126,
		UnsignedInt = 5125,
		UnsignedByte = 5121
	};

	struct LayoutElement {
		LayoutType Type;
		unsigned int Count = 1;
		bool Normalised = false;

		static unsigned int GetSize(const LayoutType& Type) {

			switch (Type) {
				case LayoutType::Float:			return 4;
				case LayoutType::UnsignedInt:	return 4;
				case LayoutType::UnsignedByte:	return 1;
				//case GL_FLOAT: return 4;
			}

			assert(false);
			return 0;
		};
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(std::initializer_list<LayoutElement> Init);

		void SetLayout(std::vector<LayoutElement> Elements);

		inline const std::vector<LayoutElement> GetElements() const& { return Elements; }
		inline unsigned int GetStride() const { return bStride; }

	private:
		std::vector<LayoutElement> Elements;
		unsigned int bStride = 0;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(unsigned int Size);
		VertexBuffer(const void* Data, unsigned int Size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void Set(const void* Data, unsigned int Size);

		inline VertexBufferLayout GetLayout() const { return aLayout; };

		void SetBufferLayout(const VertexBufferLayout& Layout);
	private:
		VertexBufferLayout aLayout;

		unsigned int RendererID;
	};
}