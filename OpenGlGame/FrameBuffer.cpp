#include "pch.h"
#include "FrameBuffer.h"

#include "Assert.h"

namespace Game
{
	void FrameBuffer::DeleteFrameBuffer(IdType *id)
	{
		glDeleteFramebuffers(1, id);
		delete id;
	}

	auto FrameBuffer::CreateFrameBuffer()
	{
		auto frameBuffer = Pointer<IdType>(new IdType{}, DeleteFrameBuffer);
		glGenFramebuffers(1, &*frameBuffer);
		return frameBuffer;
	}

	FrameBuffer::FrameBuffer(IdType id)
	{
		m_FrameBuffer = MakePointer<IdType>(id);
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
		{
			GL_LOG_WARN("Unknown clolor depth sselecting {}", colorDepth < 24 ? "24 bit" : "32 bit");

			colorBufferFormat = colorDepth < 24 ? InternalFormat::RGB : InternalFormat::RGBA;
		}

		InternalFormat depthFormat;
		switch(depthBuffer)
		{
			case 0:
				break;
			case 8:
				depthFormat = InternalFormat::DepthComponent;
				break;
			case 16:
				depthFormat = InternalFormat::DepthComponent16;
				break;
			case 24:
				depthFormat = InternalFormat::DepthComponent24;
				break;
			case 32:
				depthFormat = InternalFormat::DepthComponent32F;
				break;
			default: GL_LOG_WARN("Unknown depth buffer size reseting it to 16 bites");
				depthFormat = InternalFormat::DepthComponent16;
		}

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

	void FrameBuffer::Bind(int32_t x, int32_t y, int32_t width, int32_t height) const
	{
		Bind();
		glViewport(x, y, width, height);
	}

	void FrameBuffer::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this);
	}

	FrameBuffer* FrameBuffer::GetDefault()
	{
		static FrameBuffer frameBuffer(0);
		return &frameBuffer;
	}

	Vector2i FrameBuffer::MaxViewportSize()
	{
		static Vector2i value;

		if(value == Vector2i())
			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &value.X);

		return value;
	}
}
