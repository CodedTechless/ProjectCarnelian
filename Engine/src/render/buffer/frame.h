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

		void Invalidate();
		void Resize(const glm::u32vec2& Size);

		void Bind();
		void Unbind();

		inline FrameBufferSpecification& GetSpecification() { return Specification; };
		inline unsigned int GetColourAttachmentRendererID(unsigned int index = 0) { return ColourRenderAttachments[index]; };

	private:
		std::vector<FrameBufferFormatSpecification> ColourAttachmentSpecifications;
		std::vector<unsigned int> ColourRenderAttachments = {};

		FrameBufferFormatSpecification DepthAttachmentSpecification = FrameBufferTextureFormat::None;
		unsigned int DepthRenderAttachment = 0;

		unsigned int RendererID = 0;

		FrameBufferSpecification Specification;
	};
}
