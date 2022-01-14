
#include "frame.h"

#include <engine/watchdog/watchdog.h>

#include <GL/glew.h>

namespace Techless
{

	namespace FrameBufferUtil
	{
		static bool IsDepthFormat(FrameBufferTextureFormat format)
		{
			return format == FrameBufferTextureFormat::DEPTH24STENCIL8;
		}

		static GLenum GetSamplingTarget(int Samples)
		{
			return Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void AttachColourTexture(uint32_t ID, int Samples, GLenum InternalFormat, GLenum Format, const glm::u32vec2& Size, int Index)
		{
			bool MultisamplingEnabled = Samples > 1;
			if (MultisamplingEnabled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Size.x, Size.y, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Size.x, Size.y, 0, Format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GetSamplingTarget(Samples), ID, 0);
		}

		static void AttachDepthTexture(uint32_t ID, int Samples, GLenum Format, GLenum AttachmentType, const glm::u32vec2& Size)
		{
			bool MultisamplingEnabled = Samples > 1;
			if (MultisamplingEnabled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Size.x, Size.y, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, Format, Size.x, Size.y);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, GetSamplingTarget(Samples), ID, 0);
		}

		static void CreateTextures(int Samples, int Count, uint32_t* DataStream)
		{
			glCreateTextures(GetSamplingTarget(Samples), Count, DataStream);
		}
		
		static void BindTexture(int Samples, uint32_t TextureID)
		{
			glBindTexture(GetSamplingTarget(Samples), TextureID);
		}
	}

	FrameBuffer::FrameBuffer(const FrameBufferSpecification& spec)
		: Specification(spec) 
	{

		for (auto format : spec.Attachments.Formats)
		{
			if (!FrameBufferUtil::IsDepthFormat(format.TextureFormat))
				ColourAttachmentSpecifications.emplace_back(format.TextureFormat);
			else
				DepthAttachmentSpecification = format.TextureFormat;
		}

		Invalidate();
	}

	FrameBuffer::~FrameBuffer()
	{
		Clear();
	}

	void FrameBuffer::Clear()
	{
		glDeleteFramebuffers(1, &RendererID);
		glDeleteTextures(ColourRenderAttachments.size(), ColourRenderAttachments.data());
		glDeleteTextures(1, &DepthRenderAttachment);

		ColourRenderAttachments.clear();
		DepthRenderAttachment = 0;
		RendererID = 0;
	}

	void FrameBuffer::Invalidate()
	{
		if (RendererID)
			Clear();

		glCreateFramebuffers(1, &RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, RendererID);

		bool MultisamplingEnabled = Specification.Samples > 1;

		if (ColourAttachmentSpecifications.size())
		{
			ColourRenderAttachments.resize(ColourAttachmentSpecifications.size());
			FrameBufferUtil::CreateTextures(Specification.Samples, ColourRenderAttachments.size(), ColourRenderAttachments.data());

			for (size_t i = 0; i < ColourRenderAttachments.size(); ++i)
			{
				FrameBufferUtil::BindTexture(Specification.Samples, ColourRenderAttachments[i]);

				switch (ColourAttachmentSpecifications[i].TextureFormat)
				{
					case FrameBufferTextureFormat::RGBA8:
						FrameBufferUtil::AttachColourTexture(ColourRenderAttachments[i], Specification.Samples, GL_RGBA8, GL_RGBA, Specification.Size, i);
						break;
				}
			}
		}

		if (DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			FrameBufferUtil::CreateTextures(Specification.Samples, 1, &DepthRenderAttachment);
			FrameBufferUtil::BindTexture(Specification.Samples, DepthRenderAttachment);

			switch (DepthAttachmentSpecification.TextureFormat)
			{
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
					FrameBufferUtil::AttachDepthTexture(DepthRenderAttachment, Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, Specification.Size);
					break;
			}
		}

		if (ColourRenderAttachments.size() > 1)
		{
			assert(ColourRenderAttachments.size() <= 4);

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(ColourRenderAttachments.size(), buffers);
		}
		else if (ColourRenderAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
		glViewport(0, 0, Specification.Size.x, Specification.Size.y);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::Resize(const glm::u32vec2& Size)
	{
		//Debug::Log("Resized Frame Buffer: " + std::to_string(Specification.Size.x) + ", " + std::to_string(Specification.Size.y));

		Specification.Size = Size;

		Invalidate();
	}
}
