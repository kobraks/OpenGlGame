#include "pch.h"
#include "Framebuffer.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/Texture2D.h"

#include <glad/glad.h>

namespace Engine {
	struct Error {
		std::string_view Name;
		std::string_view Desc;
		uint32_t Code;
	};

	static constexpr Error GetErrorMessage(FramebufferObject::Status status) {
		switch (status) {
		default: return {"", ""};

		case FramebufferObject::Status::Undefined: return {
				"GL_FRAMEBUFFER_UNDEFINED", "Default framebuffer does not exists.", GL_FRAMEBUFFER_UNDEFINED
			};
		case FramebufferObject::Status::IncompleteAttachment: return {
				"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", "Framebuffer attachment points are incomplete.",
				GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
			};
		case FramebufferObject::Status::MissingAttachment: return {
				"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
				"Framebuffer does not have any image attached.",
				GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
			};
		case FramebufferObject::Status::IncompleteDrawBuffer: return {
				"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", "No color attachments for any attached buffer.",
				GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
			};
		case FramebufferObject::Status::IncompleteReadBuffer: return {
				"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", "No color attachments for any attached buffer.",
				GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
			};
		case FramebufferObject::Status::Unsupported: return {
				"GL_FRAMEBUFFER_UNSUPPORTED", "Unsupported framebuffer type.",GL_FRAMEBUFFER_UNSUPPORTED
			};
		case FramebufferObject::Status::IncompleteMultisample: return {
				"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set.", GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
			};
		case FramebufferObject::Status::IncompleteLayerTargets: return {
				"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd.",
				GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
			};
		}
	}

	static constexpr FramebufferObject::Status GlGetStatus(uint32_t name) {
		switch (glCheckNamedFramebufferStatus(name, GL_FRAMEBUFFER)) {
		case GL_FRAMEBUFFER_COMPLETE:
			return FramebufferObject::Status::Complete;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return FramebufferObject::Status::IncompleteAttachment;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return FramebufferObject::Status::MissingAttachment;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			return FramebufferObject::Status::IncompleteDrawBuffer;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			return FramebufferObject::Status::IncompleteReadBuffer;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			return FramebufferObject::Status::Unsupported;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			return FramebufferObject::Status::IncompleteMultisample;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			return FramebufferObject::Status::IncompleteLayerTargets;
		case GL_FRAMEBUFFER_UNDEFINED:
		default:
			return FramebufferObject::Status::Undefined;
		}
	}

	static GLenum GetColorFormat(int8_t depth) {
		if (depth == 24)
			return GL_RGB8;
		if (depth == 32)
			return GL_RGBA8;

		LOG_GL_WARN("Unknown color depth {}, selected: {}", depth, depth < 24 ? "24 bit" : "32 bit");
		return depth < 24 ? GL_RGB : GL_RGBA;
	}

	static GLenum GetDepthFormat(int8_t depth, bool stencil) {
		if (!stencil) {
			if (depth == 8)
				return GL_DEPTH_COMPONENT;
			if (depth == 16)
				return GL_DEPTH_COMPONENT16;
			if (depth == 24)
				return GL_DEPTH_COMPONENT24;
			if (depth == 32)
				return GL_DEPTH_COMPONENT32;

			LOG_GL_WARN("Unknown depth buffer size: '{}', Setting it to 16 bits", depth);
			return GL_DEPTH_COMPONENT16;
		}

		if (depth == 24)
			return GL_DEPTH24_STENCIL8;
		if (depth == 32)
			return GL_DEPTH32F_STENCIL8;

		LOG_GL_WARN("Unknown depth buffer size : '{}', Setting it to 24 bits and 8 Stencil bits", depth);
		return GL_DEPTH24_STENCIL8;
	}

	void FramebufferObject::Bind(const Vector2i& position, const Vector2u& size) const {
		glBindFramebuffer(GL_FRAMEBUFFER, *this);
		glViewport(position.X, position.Y, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void FramebufferObject::Bind() const {
		Bind({0, 0}, m_Internals->Size);
	}

	void FramebufferObject::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int FramebufferObject::ReadPixel(uint32_t attachmentIndex, const Vector2i& position) {
		ENGINE_ASSERT(attachmentIndex < m_Internals->ColorAttachmentCount);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(position.X, position.Y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void FramebufferObject::Invalidate() {
		m_Internals->Invalidate();
	}

	void FramebufferObject::Resize(const Vector2u& size) {
		if (size.Width == 0 || size.Height == 0 || size.Width > MaxViewportSize().Width || size.Height > MaxViewportSize().Height) {
			LOG_GL_WARN("Attempted to resize framebuffer to {0}, {1}", size.Width, size.Height);
			return;
		}

		m_Internals->Size = size;
		Invalidate();
	}

	FramebufferObject::Status FramebufferObject::GetStatus() const {
		return GlGetStatus(*this);
	}

	Vector2u FramebufferObject::MaxViewportSize() {
		static Vector2u dim;

		if (dim == Vector2u()) {
			int result = 0;

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &result);
			dim = {static_cast<uint32_t>(result), static_cast<uint32_t>(result)};
		}

		return dim;
	}

	FramebufferObject::Internals::Internals() {
		glCreateFramebuffers(1, &Id);
	}

	FramebufferObject::Internals::~Internals() {
		glDeleteFramebuffers(1, &Id);
	}

	void FramebufferObject::Internals::Invalidate() {
		glDeleteFramebuffers(1, &Id);
		glCreateFramebuffers(1, &Id);
	}

	FramebufferObject::FramebufferObject(const Vector2u& size) : m_Internals(MakeRef<Internals>()) {
		m_Internals->Size = size;
	}

	void FramebufferObject::Attach(uint32_t index, Ref<Texture2D> texture) {
		glNamedFramebufferTexture(*this, index, *texture, 0);
	}

	void FramebufferObject::Attach(uint32_t attachment, Ref<RenderBuffer> renderBuffer) {
		//not implemented
		ENGINE_ASSERT(false, "not implemented");
		throw std::exception("not implemented");
	}

	Ref<Texture2D> FramebufferObject::CreateColorTextureAttachment(const Vector2u& size, uint32_t index, uint8_t depth,
	                                                               uint32_t samples) {
		m_Internals->ColorAttachmentCount++;
		m_Internals->ColorDepth = depth;
		m_Internals->Size = size;
		m_Internals->Samples = samples;

		auto colorTexture = Texture2D::Create(size, samples);

		if (!colorTexture->IsMultisampled()) {
			colorTexture->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			colorTexture->SetFilters(Filter::Linear, Filter::Linear);
		}

		Attach(GL_COLOR_ATTACHMENT0 + index, colorTexture);

		return colorTexture;
	}

	Ref<RenderBuffer> FramebufferObject::CreateColorRenderBufferAttachment(const Vector2u& size, uint32_t index,
	                                                                       uint8_t depth, uint32_t samples) {
		//not implemented
		ENGINE_ASSERT(false, "not implemented");
		throw std::exception("not implemented");

		return nullptr;
	}

	Ref<Texture2D> FramebufferObject::CreateDepthTextureAttachment(const Vector2u& size, uint8_t depth, bool stencil, uint32_t samples) {
		if (depth == 0)
			return nullptr;

		m_Internals->DepthBufferDepth = depth;
		m_Internals->Stencil = stencil;
		m_Internals->Size = size;
		m_Internals->Samples = samples;

		auto depthTexture = Texture2D::Create(size, samples);
		if (!depthTexture->IsMultisampled()) {
			depthTexture->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			depthTexture->SetFilters(Filter::Nearest, Filter::Nearest);
		}
		
		Attach(GL_DEPTH_COMPONENT, depthTexture);

		return depthTexture;
	}

	Ref<RenderBuffer> FramebufferObject::CreateDepthRenderBufferAttachment(const Vector2u& size, uint8_t depth,
	                                                                       bool stencil, uint32_t samples) {
		//not implemented
		ENGINE_ASSERT(false, "not implemented");
		throw std::exception("not implemented");

		return nullptr;
	}

	void FramebufferObject::CheckCompleteness() const {
		const auto status = GetStatus();

		if (status != Status::Complete) {
			const auto error = GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{:#x}: {}->{}", error.Code, error.Name, error.Desc);

			ENGINE_ASSERT(false, fmt::format("Unable to create framebuffer: {}", buffer.data()));
			throw std::exception(fmt::format("Unable to create framebuffer: {}", buffer.data()).c_str());
		}
	}
}
