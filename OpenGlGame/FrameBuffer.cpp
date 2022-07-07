#include "pch.h"
#include "FrameBuffer.h"

#include <string_view>

#include "Texture.h"

#include "Assert.h"
#include "GLCheck.h"
#include "RenderBuffer.h"

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
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return {"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", "Framebuffer attachment points are incomplete."};
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return {
					"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
					"Framebuffer does not have any image attached."
				};
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return {"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", "No color attachments for any attached buffer"};
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return {"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", "No color attachments for any attached buffer"};
			case GL_FRAMEBUFFER_UNSUPPORTED: return {"GL_FRAMEBUFFER_UNSUPPORTED", "Unsupported framebuffer type"};
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return {"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set."};
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return {"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd"};
		}
	}

	static InternalFormat GetColorFormat(int8_t depth)
	{
		if(depth == 24)
			return InternalFormat::RGB8;
		if(depth == 32)
			return InternalFormat::RGBA8;

		GL_LOG_WARN("Unknown color depth {}, selected: {}", depth, depth < 24 ? "24 bit" : "32 bit");

		return depth < 24 ? InternalFormat::RGB : InternalFormat::RGBA;
	}

	static InternalFormat GetDepthFormat(int8_t depth, bool stencil)
	{
		if(!stencil)
		{
			if(depth == 8)
				return InternalFormat::DepthComponent;
			if(depth == 16)
				return InternalFormat::DepthComponent16;
			if(depth == 24)
				return InternalFormat::DepthComponent24;
			if(depth == 32)
				return InternalFormat::DepthComponent32F;

			GL_LOG_WARN("Unknown depth buffer size: '{}', Setting it to 16 bits", depth);
			return InternalFormat::DepthComponent16;
		}
		if(depth == 24)
			return InternalFormat::Depth24Stencil8;
		if(depth == 32)
			return InternalFormat::Depth32FStencil8;


		GL_LOG_WARN("Unknown depth buffer size: '{}', Setting it to 24 bits and 8 Stencil bits", depth);
		return InternalFormat::DepthComponent16;
	}

	FrameBufferObject::Internals::Internals()
	{
		
		GL_CHECK(glGenFramebuffers(1, &Id));
	}

	FrameBufferObject::Internals::Internals(IdType id)
	{
		Id = id;
	}

	FrameBufferObject::Internals::~Internals()
	{
		
		GL_CHECK(glDeleteFramebuffers(1, &Id));
	}

	FrameBufferObject::FrameBufferObject(IdType id)
	{
		m_Internals = MakePointer<Internals>(id);
	}

	FrameBufferObject::FrameBufferObject() : m_Internals(MakePointer<Internals>()) {}

	void FrameBufferObject::Bind(int32_t x, int32_t y, int32_t width, int32_t height) const
	{
		
		Bind();
		GL_CHECK(glViewport(x, y, width, height));
	}

	void FrameBufferObject::Bind() const
	{
		
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *this));
	}

	FrameBufferObject::Status FrameBufferObject::GetStatus() const
	{
		
		GLenum status = GL_FRAMEBUFFER_COMPLETE;
		GL_CHECK(status = glCheckNamedFramebufferStatus(m_Internals->Id, GL_DRAW_FRAMEBUFFER));

		return static_cast<Status>(status);
	}

	FrameBufferObject* FrameBufferObject::GetDefault()
	{
		static FrameBufferObject frameBuffer(0);
		return &frameBuffer;
	}

	Vector2i FrameBufferObject::MaxViewportSize()
	{
		static Vector2i value;

		
		if(value == Vector2i())
			GL_CHECK(glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &value.X));

		return value;
	}

	Pointer<Texture> FrameBufferObject::SetUpColorTextureAttachment(uint32_t width, uint32_t height, uint32_t index, uint8_t depth)
	{
		m_Internals->ColorAttachments++;
		
		auto attachment = MakePointer<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, width, height, GetColorFormat(depth));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Linear, Filter::Linear);

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);

		return attachment;
	}

	Pointer<RenderBuffer> FrameBufferObject::SetUpColorRenderBufferAttachment(uint32_t width, uint32_t height, uint32_t index, uint8_t depth)
	{
		m_Internals->ColorAttachments++;
		
		auto attachment = MakePointer<RenderBuffer>();

		attachment->Storage(width, height, GetColorFormat(depth));

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);
		return attachment;
	}

	Pointer<Texture> FrameBufferObject::SetUpDepthTextureAttachment(uint32_t width, uint32_t height, uint8_t depth, bool stencil)
	{
		if(depth == 0)
			return nullptr;

		auto attachment = MakePointer<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::DepthComponent, width, height, GetDepthFormat(depth, stencil));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Nearest, Filter::Nearest);
		Attach(GL_DEPTH_COMPONENT, attachment);

		return attachment;
	}

	Pointer<RenderBuffer> FrameBufferObject::SetUpDepthRenderBufferAttachment(uint32_t width, uint32_t height, uint8_t depth, bool stencil)
	{
		if(depth == 0)
			return nullptr;

		auto attachment = MakePointer<RenderBuffer>();

		attachment->Storage(width, height, GetDepthFormat(depth, stencil));

		return attachment;
	}

	void FrameBufferObject::CheckCompletion() const
	{
		
		GLenum status = GL_FRAMEBUFFER_COMPLETE;
		GL_CHECK(status = glCheckNamedFramebufferStatus(m_Internals->Id, GL_DRAW_FRAMEBUFFER));

		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			const auto error = GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{}: {} -> {}", status, error.Name, error.Desc);

			ASSERT(false, fmt::format("Unable to create framebuffer {}", buffer.data()));
			throw std::runtime_error(fmt::format("Unable to create frame buffer {}", buffer.data()));
		}
	}

	void FrameBufferObject::Attach(uint32_t attachment, Pointer<Texture> texture)
	{
		
		GL_CHECK(glNamedFramebufferTexture(m_Internals->Id, attachment, *texture, 0));
	}

	void FrameBufferObject::Attach(uint32_t attachment, Pointer<RenderBuffer> renderBuffer)
	{
		
		GL_CHECK(glNamedFramebufferRenderbuffer(m_Internals->Id, attachment, GL_RENDERBUFFER, renderBuffer->Id()));
	}
}
