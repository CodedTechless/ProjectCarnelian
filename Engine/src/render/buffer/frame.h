#pragma once

//#include <Engine.h>

#include <engineincl.h>

#include <glm/glm.hpp>

namespace Techless
{

	enum class FrameBufferTextureFormat
	{
		None = 0,

		RGBA8,
		DEPTH24STENCIL8,

		// default
		Colour = RGBA8,
		Depth = DEPTH24STENCIL8

	};

	struct FrameBufferFormatSpecification
	{
		FrameBufferFormatSpecification() = default;
		FrameBufferFormatSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format) {};

		FrameBufferTextureFormat TextureFormat;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferFormatSpecification> formats)
			: Formats(formats) {};

		std::vector<FrameBufferFormatSpecification> Formats;
	};

	struct FrameBufferSpecification
	{
		glm::u32vec2 Size;
		FrameBufferAttachmentSpecification Attachments;

		unsigned int Samples = 1;
		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecification& spec);
		~FrameBuffer();

		void Clear();
		void Invalidate();
		void Resize(const glm::u32vec2& Size);

		void Bind();
		void Unbind();

		inline FrameBufferSpecification& GetSpecification() { return Specification; };
		inline unsigned int GetColourAttachmentRendererID(size_t index = 0) { return ColourRenderAttachments[index]; };

	private:
		std::vector<FrameBufferFormatSpecification> ColourAttachmentSpecifications;
		std::vector<uint32_t> ColourRenderAttachments = {};

		FrameBufferFormatSpecification DepthAttachmentSpecification = FrameBufferTextureFormat::None;
		uint32_t DepthRenderAttachment = 0;

		uint32_t RendererID = 0;

		FrameBufferSpecification Specification;
	};
}
