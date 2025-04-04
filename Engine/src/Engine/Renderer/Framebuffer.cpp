#include "pch.h"
#include "Framebuffer.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/RenderBuffer.h"

#include <glad/glad.h>

namespace Engine {
	namespace Utils {
		struct Error {
			std::string_view Name;
			std::string_view Desc;
			uint32_t Code;
		};

		constexpr Error GetErrorMessage(FramebufferObject::Status status) {
			switch (status) {
			default: return {"", "", 0};

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
					"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set.",
					GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
				};
			case FramebufferObject::Status::IncompleteLayerTargets: return {
					"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd.",
					GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
				};
			}
		}

		constexpr FramebufferObject::Status GlGetStatus(uint32_t name) {
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

		Texture::InternalFormat ToTextureFormat(FramebufferAttachmentFormat format) {
			switch (format) {
			case FramebufferAttachmentFormat::None:
				ENGINE_ASSERT(false);
				throw std::exception();
			case FramebufferAttachmentFormat::RGBA8:
				return Texture::InternalFormat::RGBA8;
			case FramebufferAttachmentFormat::RedInteger:
				return Texture::InternalFormat::R32I;
			case FramebufferAttachmentFormat::DepthComponent:
				return Texture::InternalFormat::DepthComponent;
			case FramebufferAttachmentFormat::DepthComponent16:
				return Texture::InternalFormat::DepthComponent16;
			case FramebufferAttachmentFormat::DepthComponent24:
				return Texture::InternalFormat::DepthComponent24;
			case FramebufferAttachmentFormat::DepthComponent32:
				return Texture::InternalFormat::DepthComponent32;
			case FramebufferAttachmentFormat::Depth24Stencil8:
				return Texture::InternalFormat::Depth24Stencil8;
			case FramebufferAttachmentFormat::Depth32FStencil8:
				return Texture::InternalFormat::Depth32FStencil8;
			}
		}

		RenderBuffer::InternalFormat ToRenderBufferFormat(FramebufferAttachmentFormat format) {
			switch (format) {
			case FramebufferAttachmentFormat::None:
				ENGINE_ASSERT(false);
				throw std::exception();
			case FramebufferAttachmentFormat::RGBA8:
				return RenderBuffer::InternalFormat::RGBA8;
			case FramebufferAttachmentFormat::RedInteger:
				return RenderBuffer::InternalFormat::R32I;
			case FramebufferAttachmentFormat::DepthComponent:
				return RenderBuffer::InternalFormat::DepthComponent;
			case FramebufferAttachmentFormat::DepthComponent16:
				return RenderBuffer::InternalFormat::DepthComponent16;
			case FramebufferAttachmentFormat::DepthComponent24:
				return RenderBuffer::InternalFormat::DepthComponent24;
			case FramebufferAttachmentFormat::DepthComponent32:
				return RenderBuffer::InternalFormat::DepthComponent32;
			case FramebufferAttachmentFormat::Depth24Stencil8:
				return RenderBuffer::InternalFormat::Depth24Stencil8;
			case FramebufferAttachmentFormat::Depth32FStencil8:
				return RenderBuffer::InternalFormat::Depth32FStencil8;
			}
		}
	}

	void FramebufferObject::Bind(const Vector2i& position, const Vector2u& size) const {
		glBindFramebuffer(GL_FRAMEBUFFER, *this);
		glViewport(position.X, position.Y, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void FramebufferObject::Bind() const {
		Bind({0, 0}, m_Internals->Specification.Size);
	}

	void FramebufferObject::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int FramebufferObject::ReadPixel(uint32_t attachmentIndex, const Vector2i& position) const {
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
		if (size.Width == 0 || size.Height == 0 || size.Width > MaxViewportSize().Width || size.Height >
			MaxViewportSize().Height) {
			LOG_GL_WARN("Attempted to resize framebuffer to {0}, {1}", size.Width, size.Height);
			return;
		}

		m_Internals->Specification.Size = size;
		Invalidate();
	}

	FramebufferObject::Status FramebufferObject::GetStatus() const {
		return Utils::GlGetStatus(*this);
	}

	Vector2u FramebufferObject::MaxViewportSize() {
		static Vector2u viewportSize;

		if (viewportSize == Vector2u()) {
			int value[2];

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, value);

			viewportSize = {static_cast<uint32_t>(value[0]), static_cast<uint32_t>(value[1])};
		}

		return viewportSize;
	}

	FramebufferObject::Internals::Internals(const FramebufferSpecification& specification) : Specification(
		specification) {
		glCreateFramebuffers(1, &Id);
	}

	FramebufferObject::Internals::~Internals() {
		glDeleteFramebuffers(1, &Id);
	}

	void FramebufferObject::Internals::Invalidate() {
		glDeleteFramebuffers(1, &Id);
		glCreateFramebuffers(1, &Id);
	}

	FramebufferObject::FramebufferObject(const FramebufferSpecification& specification) : m_Internals(
		MakeRef<Internals>(specification)) {
	}

	void FramebufferObject::Attach(uint32_t index, Ref<Texture> texture) {
		glNamedFramebufferTexture(*this, index, *texture, 0);
	}

	void FramebufferObject::Attach(uint32_t attachment, Ref<RenderBuffer> renderBuffer) {
		glNamedFramebufferRenderbuffer(*this, attachment, GL_RENDERBUFFER, *renderBuffer);
	}

	Ref<Texture> FramebufferObject::CreateColorTextureAttachment(FramebufferAttachmentFormat format) {
		if (format == FramebufferAttachmentFormat::None)
			return nullptr;

		const auto index = m_Internals->ColorAttachmentCount++;

		auto texture = Texture::Create(m_Internals->Specification.Size, m_Internals->Specification.Samples,
		                               Utils::ToTextureFormat(format));

		if (!texture->IsMultisampled()) {
			texture->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			texture->SetFilters(Filter::Linear, Filter::Linear);
		}

		Attach(GL_COLOR_ATTACHMENT0 + index, texture);

		return texture;
	}

	Ref<RenderBuffer> FramebufferObject::CreateColorRenderBufferAttachment(FramebufferAttachmentFormat format) {
		if (format == FramebufferAttachmentFormat::None)
			return nullptr;

		const auto index =m_Internals->ColorAttachmentCount++;

		auto renderBuffer = RenderBuffer::Create(m_Internals->Specification.Size, m_Internals->Specification.Samples, Utils::ToRenderBufferFormat(format));

		Attach(GL_COLOR_ATTACHMENT0 + index, renderBuffer);

		return renderBuffer;
	}

	Ref<Texture> FramebufferObject::CreateDepthTextureAttachment(FramebufferAttachmentFormat format) {
		if (format == FramebufferAttachmentFormat::None)
			return nullptr;

		auto texture = Texture::Create(m_Internals->Specification.Size, m_Internals->Specification.Samples, Utils::ToTextureFormat(format));

		if (!texture->IsMultisampled()) {
			texture->SetWrapping(Wrapping::ClampEdge, Wrapping::ClampEdge);
			texture->SetFilters(Filter::Nearest, Filter::Nearest);
		}

		m_Internals->DepthBuffer = true;
		if (format > FramebufferAttachmentFormat::DepthComponent32) {
			m_Internals->Stencil = true;
			Attach(GL_DEPTH_STENCIL_ATTACHMENT, texture);
		}
		else
			Attach(GL_DEPTH_ATTACHMENT, texture);

		return texture;
	}


	Ref<RenderBuffer> FramebufferObject::CreateDepthRenderBufferAttachment(FramebufferAttachmentFormat format) {
		if (format == FramebufferAttachmentFormat::None)
			return nullptr;

		auto renderBuffer = RenderBuffer::Create(m_Internals->Specification.Size, m_Internals->Specification.Samples, Utils::ToRenderBufferFormat(format));

		m_Internals->DepthBuffer = true;
		if (format > FramebufferAttachmentFormat::DepthComponent32) {
			m_Internals->Stencil = true;
			Attach(GL_DEPTH_STENCIL_ATTACHMENT, renderBuffer);
		}
		else
			Attach(GL_DEPTH_ATTACHMENT, renderBuffer);

		return renderBuffer;
	}

	void FramebufferObject::CheckCompleteness() const {
		const auto status = GetStatus();

		if (status != Status::Complete) {
			const auto error = Utils::GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{:#x}: {}->{}", error.Code, error.Name, error.Desc);

			ENGINE_ASSERT(false, fmt::format("Unable to create framebuffer: {}", buffer.data()));
			throw std::exception(fmt::format("Unable to create framebuffer: {}", buffer.data()).c_str());
		}
	}

	void FramebufferObject::SetUpBuffers(uint32_t num) {
		if (num > 1) {
			ENGINE_ASSERT(num <= 4);

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glNamedFramebufferDrawBuffers(*this, num, buffers);
		}
		else {
			glNamedFramebufferDrawBuffer(*this, GL_NONE);
		}
	}
}
