#include "pch.h"
#include "FrameBuffer.h"

#include "Assert.h"

namespace Game
{
	static void DestroyFrameBuffer(FrameBuffer::IdType *id)
	{
		glDeleteFramebuffers(1, id);
		delete id;
	}

	static auto CreateFrameBuffer()
	{
		auto frameBuffer = Pointer<FrameBuffer::IdType>(new FrameBuffer::IdType{}, DestroyFrameBuffer);
		glGenFramebuffers(1, &*frameBuffer);
		return frameBuffer;
	}

	FrameBuffer::FrameBuffer(
		const uint32_t width,
		const uint32_t height,
		const uint8_t colorDepth,
		const uint8_t depthBuffer
		)
	{
		InternalFormat colorBufferFormat;

		if(colorDepth == 24)
			colorBufferFormat = InternalFormat::RGB;
		else if(colorDepth == 32)
			colorBufferFormat = InternalFormat::RGBA;
		else
			throw std::exception();


		InternalFormat depthFormat;
		if(depthBuffer == 8)
			depthFormat = InternalFormat::DepthComponent;
		else if(depthBuffer == 16)
			depthFormat = InternalFormat::DepthComponent16;
		else if(depthBuffer == 24)
			depthFormat = InternalFormat::DepthComponent24;
		else if(depthBuffer == 32)
			depthFormat = InternalFormat::DepthComponent32F;
		else
			throw std::exception();

		glBindTexture(GL_TEXTURE_2D, 0);

		m_FrameBuffer = CreateFrameBuffer();
		Bind();

		m_ColorBuffer = MakePointer<Texture>();
		m_ColorBuffer->Image2D(nullptr, DataType::UnsignedByte, Format::RGBA, width, height, colorBufferFormat);
		m_ColorBuffer->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		m_ColorBuffer->SetFilters(Filter::Linear, Filter::Linear);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *m_ColorBuffer, 0);

		if(depthBuffer > 0)
		{
			m_DepthBuffer = MakePointer<Texture>();
			glTexImage2D(
			             GL_TEXTURE_2D,
			             0,
			             static_cast<GLint>(depthFormat),
			             width,
			             height,
			             0,
			             GL_DEPTH_COMPONENT,
			             GL_UNSIGNED_BYTE,
			             nullptr
			            );
			m_DepthBuffer->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			m_DepthBuffer->SetFilters(Filter::Nearest, Filter::Nearest);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *m_DepthBuffer, 0);
		}

		// ASSERT(glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE. "Unable to create frame buffer");
		if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::exception();
	}

	void FrameBuffer::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this);
	}


	void FrameBuffer::Bind(int32_t width, int32_t height) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this);
		glViewport(0, 0, width, height);
	}
}
