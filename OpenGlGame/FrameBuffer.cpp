#include "pch.h"
#include "FrameBuffer.h"

#include <string_view>

#include "Assert.h"
#include "GLCheck.h"

namespace Game
{
	struct Error
	{
		std::string_view Name;
		std::string_view Desc;
	};

	static Error GetErrorMessage(GLenum errorCode)
	{
		switch(errorCode)
		{
			default: return {"", ""};

			case GL_FRAMEBUFFER_UNDEFINED: return {"GL_FRAMEBUFFER_UNDEFINED", "Default framebuffer does not exists."};
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return {
					"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT",
					"Framebuffer attachment points are incomplete."
				};
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return {
					"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
					"Framebuffer does not have any image attached."
				};
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return {
					"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER",
					"No color attachments for any attached buffer"
				};
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return {
					"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER",
					"No color attachments for any attached buffer"
				};
			case GL_FRAMEBUFFER_UNSUPPORTED: return { "GL_FRAMEBUFFER_UNSUPPORTED", "Unsupported framebuffer type"};
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return {"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set."};
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return {"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd"};
		}
	}

	void FrameBuffer::DeleteFrameBuffer(IdType *id)
	{
		GL_CHECK(glDeleteFramebuffers(1, id));
		delete id;
	}

	auto FrameBuffer::CreateFrameBuffer()
	{
		auto frameBuffer = Pointer<IdType>(new IdType{}, DeleteFrameBuffer);
		GL_CHECK(glGenFramebuffers(1, &*frameBuffer));
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

		if(colorDepth == 24) colorBufferFormat = InternalFormat::RGB;
		else if(colorDepth == 32) colorBufferFormat = InternalFormat::RGBA;
		else
		{
			GL_LOG_WARN("Unknown clolor depth sselecting {}", colorDepth < 24 ? "24 bit" : "32 bit");

			colorBufferFormat = colorDepth < 24 ? InternalFormat::RGB : InternalFormat::RGBA;
		}

		InternalFormat depthFormat;
		switch(depthBuffer)
		{
			case 0: break;
			case 8: depthFormat = InternalFormat::DepthComponent;
				break;
			case 16: depthFormat = InternalFormat::DepthComponent16;
				break;
			case 24: depthFormat = InternalFormat::DepthComponent24;
				break;
			case 32: depthFormat = InternalFormat::DepthComponent32F;
				break;
			default: GL_LOG_WARN("Unknown depth buffer size reseting it to 16 bites");
				depthFormat = InternalFormat::DepthComponent16;
		}

		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		m_FrameBuffer = CreateFrameBuffer();
		Bind();

		m_ColorBuffer = MakePointer<Texture>();
		m_ColorBuffer->Image2D(nullptr, DataType::UnsignedByte, Format::RGBA, width, height, colorBufferFormat);
		m_ColorBuffer->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		m_ColorBuffer->SetFilters(Filter::Linear, Filter::Linear);
		GL_CHECK(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *m_ColorBuffer, 0));

		if(depthBuffer > 0)
		{
			m_DepthBuffer = MakePointer<Texture>();
			GL_CHECK(
			         glTexImage2D( GL_TEXTURE_2D, 0, static_cast<GLint>(depthFormat), width, height, 0,
				         GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr )
			        );
			m_DepthBuffer->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			m_DepthBuffer->SetFilters(Filter::Nearest, Filter::Nearest);
			GL_CHECK(
			         glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *m_DepthBuffer, 0)
			        );
		}

		GLenum checkStatus = GL_FRAMEBUFFER_COMPLETE;
		GL_CHECK(checkStatus = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ));

		if(checkStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			const auto error = GetErrorMessage(checkStatus);
			fmt::memory_buffer buffer;
			format_to(buffer, "{}: {} -> {}", checkStatus, error.Name, error.Desc);

			ASSERT(false, "Unable to create frame buffer {}", buffer.data());
			throw std::runtime_error(fmt::format("Unable to create frame buffer {}", buffer.data()));
		}
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
