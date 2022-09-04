#include "pch.h"
#include "Engine/OpenGL/FrameBuffer.h"

#include "Engine/OpenGL/RenderBuffer.h"
#include "Engine/OpenGL/Texture.h"

#include <string_view>

#include "Engine/Renderer/Context.h"

namespace Game
{
	struct Error
	{
		std::string_view Name;
		std::string_view Desc;
	};

	static constexpr Error GetErrorMessage(GLenum errorCode)
	{
		switch(errorCode)
		{
			default:
				return {"", ""};

			case GL_FRAMEBUFFER_UNDEFINED:
				return {"GL_FRAMEBUFFER_UNDEFINED", "Default framebuffer does not exists."};
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				return {"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", "Framebuffer attachment points are incomplete."};
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				return {
					"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
					"Framebuffer does not have any image attached."
				};
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				return {"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", "No color attachments for any attached buffer"};
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				return {"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", "No color attachments for any attached buffer"};
			case GL_FRAMEBUFFER_UNSUPPORTED:
				return {"GL_FRAMEBUFFER_UNSUPPORTED", "Unsupported framebuffer type"};
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				return {"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set."};
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				return {"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd"};
		}
	}

	static constexpr InternalFormat GetColorFormat(int8_t depth)
	{
		if(depth == 24)
			return InternalFormat::RGB8;
		if(depth == 32)
			return InternalFormat::RGBA8;

		GL_LOG_WARN("Unknown color depth {}, selected: {}", depth, depth < 24 ? "24 bit" : "32 bit");

		return depth < 24 ? InternalFormat::RGB : InternalFormat::RGBA;
	}

	static constexpr InternalFormat GetDepthFormat(int8_t depth, bool stencil)
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
		Functions = Context::GetContext()->GetFunctions();
		ID = Functions.GenFrameBuffer();
	}

	FrameBufferObject::Internals::Internals(IDType id)
	{
		m_Provided = true;
		Functions = Context::GetContext()->GetFunctions();
		ID = id;
	}

	FrameBufferObject::Internals::~Internals()
	{
		if (!m_Provided)
			Functions.DeleteFrameBuffer(ID);
	}

	FrameBufferObject::FrameBufferObject(IDType id) : m_Internals(MakePointer<Internals>(id)) {}
	FrameBufferObject::FrameBufferObject() : m_Internals(MakePointer<Internals>()) {}

	void FrameBufferObject::Bind(int32_t x, int32_t y, uint32_t width, uint32_t height) const
	{
		m_Internals->Functions.SetViewPort(x, y, width, height);
		Bind();
	}

	void FrameBufferObject::Bind(const Vector2i &position, Vector2u size) const
	{
		m_Internals->Functions.SetViewPort(position, size);
		Bind();
	}

	void FrameBufferObject::Bind() const
	{
		m_Internals->Functions.BindTexture(TextureTarget::Texture2D, 0);
		m_Internals->Functions.BindFrameBuffer(*this);
	}

	FrameBufferObject::Status FrameBufferObject::GetStatus() const
	{
		GLenum status = GL_FRAMEBUFFER_COMPLETE;

		status = m_Internals->Functions.CheckFrameBufferStatus(*this, GL_DRAW_FRAMEBUFFER);

		return static_cast<Status>(status);
	}

	FrameBufferObject* FrameBufferObject::GetDefault()
	{
		static FrameBufferObject s_BufferObject(0);

		return &s_BufferObject;
	}

	Vector2u FrameBufferObject::MaxViewportSize()
	{
		static Vector2u value;

		if (value == Vector2u{})
		{
			value.X = Context::GetContext()->GetFunctions().GetInteger(GL_MAX_VIEWPORT_DIMS, 0);
			value.Y = Context::GetContext()->GetFunctions().GetInteger(GL_MAX_VIEWPORT_DIMS, 1);
		}

		return value;
	}

	void FrameBufferObject::Attach(uint32_t attachment, Ref<Texture> texture)
	{
		m_Internals->Functions.FrameBufferTexture(*this, attachment, *texture, 0);
	}

	void FrameBufferObject::Attach(uint32_t attachment, Ref<RenderBuffer> buffer)
	{
		m_Internals->Functions.FrameBufferRenderBuffer(*this, attachment, GL_RENDERBUFFER, *buffer);
	}

	Ref<Texture> FrameBufferObject::SetUpColorTextureAttachment(
		uint32_t width,
		uint32_t height,
		uint32_t index,
		uint8_t depth
		)
	{
		m_Internals->ColorAttachments++;
		auto attachment = MakeRef<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, width, height, GetColorFormat(depth));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Linear, Filter::Linear);

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);

		return attachment;
	}

	Ref<Texture> FrameBufferObject::SetUpColorTextureAttachment(const Vector2u &size, uint32_t index, uint8_t depth)
	{
		m_Internals->ColorAttachments++;
		auto attachment = MakeRef<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, size, GetColorFormat(depth));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Linear, Filter::Linear);

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);

		return attachment;
	}

	Ref<RenderBuffer> FrameBufferObject::SetUpColorRenderBufferAttachment(
		uint32_t width,
		uint32_t height,
		uint32_t index,
		uint8_t depth
		)
	{
		m_Internals->ColorAttachments++;
		auto attachment = MakeRef<RenderBuffer>();

		attachment->Storage(width, height, GetColorFormat(depth));

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);
		return attachment;
	}

	Ref<RenderBuffer> FrameBufferObject::SetUpColorRenderBufferAttachment(
		const Vector2u &size,
		uint32_t index,
		uint8_t depth
		)
	{
		m_Internals->ColorAttachments++;
		auto attachment = MakeRef<RenderBuffer>();

		attachment->Storage(size, GetColorFormat(depth));

		Attach(GL_COLOR_ATTACHMENT0 + index, attachment);
		return attachment;
	}

	Ref<Texture> FrameBufferObject::SetUpDepthTextureAttachment(
		uint32_t width,
		uint32_t height,
		uint8_t depth,
		bool stencil
		)
	{
		if (depth == 0)
			return nullptr;

		auto attachment = MakeRef<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::DepthComponent, width, height, GetDepthFormat(depth, stencil));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Nearest, Filter::Nearest);
		Attach(GL_DEPTH_COMPONENT, attachment);

		return attachment;
	}

	Ref<Texture> FrameBufferObject::SetUpDepthTextureAttachment(const Vector2u &size, uint8_t depth, bool stencil)
	{
		if (depth == 0)
			return nullptr;

		auto attachment = MakeRef<Texture>();

		attachment->Image2D(nullptr, DataType::UnsignedByte, Format::DepthComponent, size, GetDepthFormat(depth, stencil));
		attachment->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
		attachment->SetFilters(Filter::Nearest, Filter::Nearest);
		Attach(GL_DEPTH_COMPONENT, attachment);

		return attachment;
	}

	Ref<RenderBuffer> FrameBufferObject::SetUpDepthRenderBufferAttachment(
		uint32_t width,
		uint32_t height,
		uint8_t depth,
		bool stencil
		)
	{
		if (depth == 0)
			return nullptr;

		auto attachment = MakeRef<RenderBuffer>();

		attachment->Storage(width, height, GetDepthFormat(depth, stencil));

		return attachment;
	}

	Ref<RenderBuffer> FrameBufferObject::SetUpDepthRenderBufferAttachment(
		const Vector2u &size,
		uint8_t depth,
		bool stencil
		)
	{
		if (depth == 0)
			return nullptr;

		auto attachment = MakeRef<RenderBuffer>();

		attachment->Storage(size, GetDepthFormat(depth, stencil));

		return attachment;
	}

	void FrameBufferObject::CheckCompletion() const
	{
		const uint32_t status = m_Internals->Functions.CheckFrameBufferStatus(*this, GL_DRAW_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			const auto error = GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{}: {} -> {}", status, error.Name, error.Desc);

			ASSERT(false, fmt::format("Unable to create frame buffer {}", buffer.data()));
			throw std::runtime_error(fmt::format("Unable to create frame buffer {}", buffer.data()));
		}
	}
}
