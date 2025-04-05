#include "pch.h"
#include "Framebuffer.h"

#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/RenderBuffer.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		struct Error {
			std::string_view Name;
			std::string_view Desc;
			uint32_t Code;
		};

		constexpr Error GetErrorMessage(Framebuffer::Status status) {
			switch (status) {
			default: return { "", "", 0 };

			case Framebuffer::Status::Undefined: return {
					"GL_FRAMEBUFFER_UNDEFINED", "Default framebuffer does not exists.", GL_FRAMEBUFFER_UNDEFINED
			};
			case Framebuffer::Status::IncompleteAttachment: return {
					"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT", "Framebuffer attachment points are incomplete.",
					GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
			};
			case Framebuffer::Status::MissingAttachment: return {
					"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
					"Framebuffer does not have any image attached.",
					GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
			};
			case Framebuffer::Status::IncompleteDrawBuffer: return {
					"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER", "No color attachments for any attached buffer.",
					GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
			};
			case Framebuffer::Status::IncompleteReadBuffer: return {
					"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER", "No color attachments for any attached buffer.",
					GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
			};
			case Framebuffer::Status::Unsupported: return {
					"GL_FRAMEBUFFER_UNSUPPORTED", "Unsupported framebuffer type.",GL_FRAMEBUFFER_UNSUPPORTED
			};
			case Framebuffer::Status::IncompleteMultisample: return {
					"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE", "Not same samples set.",
					GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
			};
			case Framebuffer::Status::IncompleteLayerTargets: return {
					"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS", "Any of attachment is layerd.",
					GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
			};
			}
		}

		constexpr Framebuffer::Status GlGetStatus(uint32_t name) {
			switch (glCheckNamedFramebufferStatus(name, GL_FRAMEBUFFER)) {
			case GL_FRAMEBUFFER_COMPLETE:
				return Framebuffer::Status::Complete;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				return Framebuffer::Status::IncompleteAttachment;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				return Framebuffer::Status::MissingAttachment;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				return Framebuffer::Status::IncompleteDrawBuffer;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				return Framebuffer::Status::IncompleteReadBuffer;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				return Framebuffer::Status::Unsupported;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				return Framebuffer::Status::IncompleteMultisample;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				return Framebuffer::Status::IncompleteLayerTargets;
			case GL_FRAMEBUFFER_UNDEFINED:
			default:
				return Framebuffer::Status::Undefined;
			}
		}

		static std::vector<uint32_t> PrepareBuffersTable() {
			static std::vector<uint32_t> buffers;

			if (buffers.empty()) {
				const auto maxElements = std::min(Framebuffer::GetMaxDrawBuffers(), Framebuffer::GetMaxColorAttachments());

				buffers.resize(maxElements);

				for (uint32_t i = 0; i < maxElements; ++i) {
					buffers[i] = GL_COLOR_ATTACHMENT0 + i;
				}
			}

			return buffers;
		}

		constexpr Texture::InternalFormat ToTextureFormat(FramebufferAttachmentFormat format) {
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

		constexpr RenderBuffer::InternalFormat ToRenderBufferFormat(FramebufferAttachmentFormat format) {
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

		constexpr Texture::Filter ToTextureFilter(FramebufferTextureFiltering filter, bool Depth = false) {
			switch (filter) {
			case FramebufferTextureFiltering::Default:
				return Depth ? Texture::Filter::Nearest : Texture::Filter::Linear;
			case FramebufferTextureFiltering::Nearest:
				return Texture::Filter::Nearest;
			case FramebufferTextureFiltering::Linear:
				return Texture::Filter::Linear;
			case FramebufferTextureFiltering::NearestMipmapNearest:
				return Texture::Filter::NearestMipmapNearest;
			case FramebufferTextureFiltering::LinearMipmapNearest:
				return Texture::Filter::LinearMipmapNearest;
			case FramebufferTextureFiltering::NearestMipmapLinear:
				return Texture::Filter::NearestMipmapLinear;
			case FramebufferTextureFiltering::LinearMipmapLinear:
				return Texture::Filter::LinearMipmapLinear;
			}
		}

		constexpr Texture::Wrapping ToTextureWrapping(FramebufferTextureWrapping wrapping) {
			switch (wrapping) {
			case FramebufferTextureWrapping::Default:
				return Texture::Wrapping::ClampEdge;
			case FramebufferTextureWrapping::Repeat:
				return Texture::Wrapping::Repeat;
			case FramebufferTextureWrapping::ClampEdge:
				return Texture::Wrapping::ClampEdge;
			case FramebufferTextureWrapping::ClampBorder:
				return Texture::Wrapping::ClampBorder;
			case FramebufferTextureWrapping::MirroredRepeat:
				return Texture::Wrapping::MirroredRepeat;
			}
		}
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification specification) {
		Ref<Framebuffer> framebuffer = Ref<Framebuffer>(new Framebuffer(specification));

		framebuffer->SetUpAttachments();
		framebuffer->CheckCompleteness();
		framebuffer->SetDrawBuffers(framebuffer->GetColorAttachmentCount() - 1);

		return framebuffer;
	}

	void Framebuffer::Bind(bool adjustViewport) const {
		glBindFramebuffer(GL_FRAMEBUFFER, *this);

		if (adjustViewport) {
			SetViewport({ 0, 0 }, m_Internals->Specification.Size);
		}
	}

	void Framebuffer::SetViewport(const Vector2u& size) const {
		SetViewport({ 0, 0 }, m_Internals->Specification.Size);
	}

	void Framebuffer::SetViewport(const Vector2i& position, const Vector2u& size) const {
		glViewport(position.X, position.Y, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void Framebuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int Framebuffer::ReadPixel(uint32_t attachmentIndex, const Vector2i& position) const {
		ENGINE_ASSERT(attachmentIndex < m_Internals->ColorAttachmentCount);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(position.X, position.Y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void Framebuffer::Invalidate() {
		m_Internals->Invalidate();
		SetUpAttachments();
		SetDrawBuffers(m_Internals->ColorAttachmentCount - 1);
	}

	void Framebuffer::Resize(const Vector2u& size) {
		if (size.Width == 0 || size.Height == 0 || size.Width > MaxViewportSize().Width || size.Height >
			MaxViewportSize().Height) {
			LOG_GL_WARN("Attempted to resize framebuffer to {0}, {1}", size.Width, size.Height);
			return;
		}

		m_Internals->Specification.Size = size;
		Invalidate();
	}

	Framebuffer::Status Framebuffer::GetStatus() const {
		return Utils::GlGetStatus(*this);
	}

	void Framebuffer::SetDrawBuffers(uint32_t drawBuffers) {
		if (drawBuffers > 1) {
			ENGINE_ASSERT(drawBuffers <= GetMaxDrawBuffers());

			const auto buffers = Utils::PrepareBuffersTable();
			ENGINE_ASSERT(drawBuffers < buffers.size());

			if (drawBuffers > m_Internals->ColorAttachmentCount) {
				ENGINE_ASSERT(false);

				throw std::runtime_error(fmt::format(
					"SetDrawBuffers failed: Number of draw buffers ({}) exceeds available color attachments ({})",
					drawBuffers, m_Internals->ColorAttachmentCount));
			}

			if (drawBuffers >= buffers.size())
				throw std::runtime_error(fmt::format("SetDrawBuffers failed: Number of draw buffers ({}) exceeds available draw buffers count ({})", drawBuffers, buffers.size()).c_str());

			glNamedFramebufferDrawBuffers(*this, static_cast<GLsizei>(drawBuffers), buffers.data());
		} else {
			if (m_Internals->ColorAttachmentCount > 0)
				glNamedFramebufferDrawBuffer(*this, GL_COLOR_ATTACHMENT0);
			else
				glNamedFramebufferDrawBuffer(*this, GL_NONE);
		}
	}

	Ref<Texture> Framebuffer::GetColorTextureAttachment(uint32_t attachmentIndex) const {
		auto attachment = GetColorAttachment(attachmentIndex);
		if (auto texture = std::get_if<Ref<Texture>>(&attachment)) {
			return *texture;
		}

		ENGINE_ASSERT(false);
		throw std::runtime_error("Attachment is not a texture");
	}

	Ref<RenderBuffer> Framebuffer::GetColorRenderBufferAttachment(uint32_t attachmentIndex) const {
		auto attachment = GetColorAttachment(attachmentIndex);
		if (auto renderBuffer = std::get_if<Ref<RenderBuffer>>(&attachment)) {
			return *renderBuffer;
		}

		ENGINE_ASSERT(false);
		throw std::runtime_error("Attachment is not a RenderBuffer");
	}

	Framebuffer::AttachmentType Framebuffer::GetColorAttachment(uint32_t attachmentIndex) const {
		if (attachmentIndex < m_Internals->ColorAttachments.size()) {
			return m_Internals->ColorAttachments[attachmentIndex];
		}

		ENGINE_ASSERT(false);
		throw std::out_of_range("Invalid attachment index");
	}

	Ref<Texture> Framebuffer::GetDepthTextureAttachment() const {
		auto attachment = GetDepthAttachment();
		if (auto texture = std::get_if<Ref<Texture>>(&attachment)) {
			return *texture;
		}

		ENGINE_ASSERT(false);
		throw std::runtime_error("Depth attachment is not a texture");
	}

	Ref<RenderBuffer> Framebuffer::GetDepthRenderBufferAttachment() const {
		auto attachment = GetDepthAttachment();
		if (auto renderBuffer = std::get_if<Ref<RenderBuffer>>(&attachment)) {
			return *renderBuffer;
		}

		ENGINE_ASSERT(false);
		throw std::runtime_error("Depth attachment is not a RenderBuffer");
	}

	Framebuffer::AttachmentType Framebuffer::GetDepthAttachment() const {
		if (m_Internals->DepthBuffer)
			return m_Internals->DepthAttachment;

		ENGINE_ASSERT(false);
		throw std::runtime_error("No depth buffer attachment");
	}

	Vector2u Framebuffer::MaxViewportSize() {
		static Vector2u viewportSize;

		if (viewportSize == Vector2u()) {
			int value[2];

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, value);

			viewportSize = { static_cast<uint32_t>(value[0]), static_cast<uint32_t>(value[1]) };
		}

		return viewportSize;
	}

	uint32_t Framebuffer::GetMaxColorAttachments() {
		uint32_t maxAttachmentCount = 0;

		if (maxAttachmentCount == 0) {
			int value = 0;

			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &value);
			maxAttachmentCount = static_cast<uint32_t>(value);
		}

		return maxAttachmentCount;
	}

	uint32_t Framebuffer::GetMaxDrawBuffers() {
		uint32_t maxDrawBuffers = 0;

		if (maxDrawBuffers == 0) {
			int value = 0;

			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &value);
			maxDrawBuffers = static_cast<uint32_t>(value);
		}

		return maxDrawBuffers;
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specification) : m_Internals(MakeRef<Internals>(specification)) {}

	void Framebuffer::CheckCompleteness() const {
		const auto status = GetStatus();

		if (status != Status::Complete) {
			const auto error = Utils::GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{:#x}: '{}'->{}", error.Code, error.Name, error.Desc);

			ENGINE_ASSERT(false, fmt::format("Unable to create framebuffer: {}", buffer.data()));
			throw std::exception(fmt::format("Unable to create framebuffer: {}", buffer.data()).c_str());
		}
	}

	void Framebuffer::SetUpAttachments() {
		const auto& attachmentSpecifications = GetSpecification().Attachments;

		for (const auto& spec : attachmentSpecifications) {
			if (spec.Format != FramebufferAttachmentFormat::None) {
				if (spec.Format > FramebufferAttachmentFormat::RedInteger)
					m_Internals->CreateDepthAttachment(spec);
				else
					m_Internals->CreateColorAttachment(spec);
			}
		}
	}

	Framebuffer::Internals::Internals(const FramebufferSpecification& specification) : Specification(specification) {
		glCreateFramebuffers(1, &ID);
	}

	Framebuffer::Internals::~Internals() {
		glDeleteFramebuffers(1, &ID);
	}

	void Framebuffer::Internals::Invalidate() {
		glDeleteFramebuffers(1, &ID);
		ColorAttachmentCount = 0;
		glCreateFramebuffers(1, &ID);
	}

	void Framebuffer::Internals::CheckStatus() const {
	}

	void Framebuffer::Internals::Attach(uint32_t attachmentPoint, Ref<Texture> attachment) {
		glNamedFramebufferTexture(ID, attachmentPoint, *attachment, 0);
		ColorAttachments.push_back(attachment);
	}

	void Framebuffer::Internals::Attach(uint32_t attachmentPoint, Ref<RenderBuffer> attachment) {
		glNamedFramebufferRenderbuffer(ID, attachmentPoint, GL_RENDERBUFFER, *attachment);
		ColorAttachments.push_back(attachment);
	}

	void Framebuffer::Internals::CreateColorAttachment(
		const FramebufferAttachmentSpecification& attachmentSpecification) {
		if (attachmentSpecification.Format == FramebufferAttachmentFormat::None)
			return;

		const auto attachmentPoint = ColorAttachmentCount++;
		ENGINE_ASSERT(attachmentPoint < GetMaxColorAttachments());

		if (attachmentSpecification.Type == FramebufferAttachmentType::RenderBuffer) {
			Attach(GL_COLOR_ATTACHMENT0 + attachmentPoint, CreateColorRenderBufferAttachment(attachmentSpecification));
		}
		else {
			Attach(GL_COLOR_ATTACHMENT0 + attachmentPoint, CreateColorTextureAttachment(attachmentSpecification));
		}
	}

	void Framebuffer::Internals::CreateDepthAttachment(
		const FramebufferAttachmentSpecification& attachmentSpecification) {
		if (attachmentSpecification.Format == FramebufferAttachmentFormat::None)
			return;

		if (attachmentSpecification.Type == FramebufferAttachmentType::RenderBuffer) {
			AttachDepth(attachmentSpecification.Format, CreateDepthRenderBufferAttachment(attachmentSpecification));
		}
		else {
			AttachDepth(attachmentSpecification.Format, CreateDepthTextureAttachment(attachmentSpecification));
		}
	}

	Ref<Texture> Framebuffer::Internals::CreateColorTextureAttachment(
		const FramebufferAttachmentSpecification& TextureSpecification) const {

		auto texture = Texture::Create(Specification.Size, Specification.Samples, Utils::ToTextureFormat(TextureSpecification.Format));

		if (!texture->IsMultisampled()) {
			texture->SetWrapping(Utils::ToTextureWrapping(TextureSpecification.TextureWrappingS), Utils::ToTextureWrapping(TextureSpecification.TextureWrappingT));
			texture->SetFilters(Utils::ToTextureFilter(TextureSpecification.TextureFilterMin), Utils::ToTextureFilter(TextureSpecification.TextureFilterMag));
		}

		return texture;
	}

	Ref<RenderBuffer> Framebuffer::Internals::CreateColorRenderBufferAttachment(
		const FramebufferAttachmentSpecification& renderBufferSpecification) const {
		return RenderBuffer::Create(Specification.Size, Specification.Samples, Utils::ToRenderBufferFormat(renderBufferSpecification.Format));
	}

	Ref<Texture> Framebuffer::Internals::CreateDepthTextureAttachment(
		const FramebufferAttachmentSpecification& textureSpecification) const {

		auto texture = Texture::Create(Specification.Size, Specification.Samples, Utils::ToTextureFormat(textureSpecification.Format));

		if (!texture->IsMultisampled()) {
			texture->SetWrapping(Utils::ToTextureWrapping(textureSpecification.TextureWrappingS), Utils::ToTextureWrapping(textureSpecification.TextureWrappingT));
			texture->SetFilters(Utils::ToTextureFilter(textureSpecification.TextureFilterMin, true), Utils::ToTextureFilter(textureSpecification.TextureFilterMag, true));
		}

		return texture;
	}

	Ref<RenderBuffer> Framebuffer::Internals::CreateDepthRenderBufferAttachment(
		const FramebufferAttachmentSpecification& renderBufferSpecification) const {
		return RenderBuffer::Create(Specification.Size, Specification.Samples, Utils::ToRenderBufferFormat(renderBufferSpecification.Format));
	}

	uint32_t Framebuffer::Internals::DepthAttachmentPoint(FramebufferAttachmentFormat format) {
		DepthBuffer = true;
		if (format > FramebufferAttachmentFormat::DepthComponent32) {
			Stencil = true;
			return GL_DEPTH_STENCIL_ATTACHMENT;
		}

		return GL_DEPTH_ATTACHMENT;
	}

	void Framebuffer::Internals::AttachDepth(FramebufferAttachmentFormat format, Ref<Texture> attachment) {
		Attach(DepthAttachmentPoint(format), attachment);
		DepthAttachment = attachment;
	}

	void Framebuffer::Internals::AttachDepth(FramebufferAttachmentFormat format, Ref<RenderBuffer> attachment) {
		Attach(DepthAttachmentPoint(format), attachment);
		DepthAttachment = attachment;
	}
}
