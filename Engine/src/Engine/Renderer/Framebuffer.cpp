#include "pch.h"
#include "Framebuffer.h"

#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/RenderBuffer.h"

#include "Engine/Utils/Renderer/ImageFormatTraits.h"
#include "Engine/Utils/Renderer/GLEnumConverters.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"

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
			default: return {"", "", 0};

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
				const auto maxElements = std::min(Framebuffer::MaxDrawBuffersCount(),
				                                  Framebuffer::MaxColorAttachmentsCount());

				buffers.resize(maxElements);

				for (uint32_t i = 0; i < maxElements; ++i) {
					buffers[i] = GL_COLOR_ATTACHMENT0 + i;
				}
			}

			return buffers;
		}

		struct DepthAttachmentInfo {
			uint32_t AttachmentPoint;
			bool HasDepth = false;
			bool HasStencil = false;
		};

		static DepthAttachmentInfo ToGLDepthAttachmentPoint(ImageFormat format) {
			DepthAttachmentInfo info;

			ENGINE_ASSERT(Utils::IsDepthFormat(format), "Non-depth format used in depth attachment!");
			if (!IsDepthFormat(format))
				throw std::invalid_argument("Format is not a depth/stencil format");

			if (format <= ImageFormat::Depth32FStencil8) {
				info.AttachmentPoint = GL_DEPTH_STENCIL_ATTACHMENT;
				info.HasDepth = true;
				info.HasStencil = true;
			}
			else if (format <= ImageFormat::StencilIndex16) {
				info.AttachmentPoint = GL_STENCIL_ATTACHMENT;
				info.HasStencil = true;
			}
			else {
				info.AttachmentPoint = GL_DEPTH_ATTACHMENT;
				info.HasDepth = true;
			}

			return info;
		}

		static std::string AttachmentPointToString(uint32_t point) {
			if (point >= GL_COLOR_ATTACHMENT0 && point <= GL_COLOR_ATTACHMENT31)
				return fmt::format("GL_COLOR_ATTACHMENT{}", point - GL_COLOR_ATTACHMENT0);

			switch (point) {
			case GL_DEPTH_ATTACHMENT: return "GL_DEPTH_ATTACHMENT";
			case GL_DEPTH_STENCIL_ATTACHMENT: return "GL_DEPTH_STENCIL_ATTACHMENT";
			case GL_STENCIL_ATTACHMENT: return "GL_STENCIL_ATTACHMENT";
			default: return fmt::format("0x{:X}", point);
			}
		}
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification) {
		auto framebuffer = Ref<Framebuffer>(new Framebuffer(specification));

		framebuffer->CreateFramebuffer();

		return framebuffer;
	}

	void Framebuffer::Bind(bool adjustViewport) const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_Internals->Specification.SwapchainTarget ? 0u : static_cast<IDType>(*this));

		if (adjustViewport) {
			SetViewport({0, 0}, m_Internals->Specification.Size);
		}
	}

	void Framebuffer::SetViewport(const Vector2u& size) const {
		SetViewport({0, 0}, size);
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
		CreateFramebuffer();
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
		return m_Internals->Status;
	}

	void Framebuffer::SetDrawBuffers(uint32_t drawBuffers) {
		if (drawBuffers > 1) {
			ENGINE_ASSERT(drawBuffers <= MaxDrawBuffersCount());

			const auto buffers = Utils::PrepareBuffersTable();
			ENGINE_ASSERT(drawBuffers < buffers.size());

			if (drawBuffers > m_Internals->ColorAttachmentCount) {
				ENGINE_ASSERT(false);

				throw std::runtime_error(fmt::format(
					"SetDrawBuffers failed: Number of draw buffers ({}) exceeds available color attachments ({})",
					drawBuffers, m_Internals->ColorAttachmentCount));
			}

			if (drawBuffers >= buffers.size())
				throw std::runtime_error(
					fmt::format(
						"SetDrawBuffers failed: Number of draw buffers ({}) exceeds available draw buffers count ({})",
						drawBuffers, buffers.size()).c_str());

			glNamedFramebufferDrawBuffers(static_cast<IDType>(*this), static_cast<GLsizei>(drawBuffers), buffers.data());
		}
		else {
			if (m_Internals->ColorAttachmentCount > 0)
				glNamedFramebufferDrawBuffer(static_cast<IDType>(*this), GL_COLOR_ATTACHMENT0);
			else
				glNamedFramebufferDrawBuffer(static_cast<IDType>(*this), GL_NONE);
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

	void Framebuffer::Present(const Ref<Framebuffer>& source, BlitMask mask, BlitFilter filter) {
		ENGINE_ASSERT(m_Internals->Specification.SwapchainTarget,
		              "Present() must be called on a swapchain framebuffer");
		ENGINE_ASSERT(source->GetSpecification().AllowBlit, "Source framebuffer must have AllowBlit = true");

		glBlitNamedFramebuffer(static_cast<IDType>(*source), 0, 0, 0, static_cast<GLint>(source->Width()),
		                       static_cast<GLint>(source->Height()), 0, 0, static_cast<GLint>(Width()),
		                       static_cast<GLint>(Height()), Utils::EnumToGLConstant(mask), Utils::EnumToGLConstant(filter));
	}

	void Framebuffer::BlitTo(const Ref<Framebuffer>& target, BlitMask mask, BlitFilter filter) {
		ENGINE_ASSERT(m_Internals->Specification.AllowBlit, "Source framebuffer must allow blit!");
		ENGINE_ASSERT(target->GetSpecification().AllowBlit, "target framebuffer must allow blit!");
		ENGINE_ASSERT(target->GetSpecification().SwapchainTarget, "Use Present() for blitting to swapchain!");

		glBlitNamedFramebuffer(static_cast<IDType>(*this), static_cast<IDType>(*target), 0, 0, static_cast<GLint>(target->Width()),
		                       static_cast<GLint>(target->Height()), 0, 0, static_cast<GLint>(Width()),
		                       static_cast<GLint>(Height()), Utils::EnumToGLConstant(mask), Utils::EnumToGLConstant(filter));
	}

	Vector2u Framebuffer::MaxViewportSize() {
		static Vector2u viewportSize;

		if (viewportSize == Vector2u()) {
			int value[2];

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, value);

			viewportSize = {static_cast<uint32_t>(value[0]), static_cast<uint32_t>(value[1])};
		}

		return viewportSize;
	}

	uint32_t Framebuffer::MaxColorAttachmentsCount() {
		uint32_t maxAttachmentCount = 0;

		if (maxAttachmentCount == 0) {
			int value = 0;

			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &value);
			maxAttachmentCount = static_cast<uint32_t>(value);
		}

		return maxAttachmentCount;
	}

	uint32_t Framebuffer::MaxDrawBuffersCount() {
		uint32_t maxDrawBuffers = 0;

		if (maxDrawBuffers == 0) {
			int value = 0;

			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &value);
			maxDrawBuffers = static_cast<uint32_t>(value);
		}

		return maxDrawBuffers;
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specification) : m_Internals(
		MakeRef<Internals>(specification)) {
	}

	void Framebuffer::CreateFramebuffer() {
		const auto& specs = m_Internals->Specification;

		if (specs.SwapchainTarget)
			return;

		if (!specs.Label.empty())
			glObjectLabel(GL_FRAMEBUFFER, static_cast<IDType>(*this), -1, specs.Label.c_str());

		SetUpAttachments();
		SetDrawBuffers(m_Internals->ColorAttachmentCount);
		CheckCompleteness();

		LOG_GL_INFO("Framebuffer '{}' created with {} color and {} depth attachments",
			Label(),
			m_Internals->ColorAttachmentCount,
			m_Internals->DepthBuffer ? 1 : 0);
	}

	void Framebuffer::CheckCompleteness() const {
		const auto status = m_Internals->CheckStatus();

		if (status != Status::Complete) {
			const auto error = Utils::GetErrorMessage(status);

			fmt::memory_buffer buffer;
			fmt::format_to(std::back_inserter(buffer), "{:#x}: '{}'->{}\0", error.Code, error.Name, error.Desc);

			LOG_GL_ERROR("Framebuffer '{}' incomplete: {} - {}", Label(), error.Name, error.Desc);
			ENGINE_ASSERT(false, fmt::format("Unable to create framebuffer: {}", buffer.data()));
			throw std::exception(fmt::format("Unable to create framebuffer: {}", buffer.data()).c_str());
		}

		LOG_GL_DEBUG("Framebuffer '{}' is complete", Label());
	}

	void Framebuffer::SetUpAttachments() {
		const auto& specs = GetSpecification();
		const auto& colorSpecs = specs.ColorAttachments;

		for (const auto& colorAttachment : colorSpecs) {
			CreateColorAttachment(colorAttachment);
		}

		if (specs.DepthAttachment)
			CreateDepthAttachment(specs.DepthAttachment.value());
	}

	void Framebuffer::CreateColorAttachment(const FramebufferAttachmentSpecification& specification) {
		const auto attachmentPoint = m_Internals->ColorAttachmentCount++;
		ENGINE_ASSERT(attachmentPoint < MaxColorAttachmentsCount());
		if (attachmentPoint >= MaxColorAttachmentsCount())
			throw std::runtime_error(
				fmt::format("Failed to create next color attachment ({}) exceeded available attachment points ({})",
				            attachmentPoint, MaxColorAttachmentsCount()).c_str());

		if (std::get_if<FramebufferRenderBufferAttachmentSpecification>(&specification)) {
			const auto& specs = *std::get_if<FramebufferRenderBufferAttachmentSpecification>(&specification);
			auto attachment = CreateAttachment(specs);
			Attach(GL_COLOR_ATTACHMENT0 + attachmentPoint, attachment);

			m_Internals->ColorAttachments.emplace_back(attachment);
		}
		else { //if its not RenderBuffer its must be Texture
			auto specs = std::get<FramebufferTextureAttachmentSpecification>(specification);
			if (specs.UseSRGB && specs.Format == ImageFormat::RGBA8)
				specs.Format = ImageFormat::SRGB8A8;

			auto attachment = CreateAttachment(specs);

			FinalizeAttachment(attachment, specs, false, attachmentPoint);
		}
	}

	void Framebuffer::CreateDepthAttachment(const FramebufferAttachmentSpecification& specification) {
		if (std::get_if<FramebufferRenderBufferAttachmentSpecification>(&specification)) {
			const auto& specs = *std::get_if<FramebufferRenderBufferAttachmentSpecification>(&specification);
			auto attachment = CreateAttachment(specs);

			AttachDepth(specs.Format, attachment);
		}
		else {
			const auto& specs = *std::get_if<FramebufferTextureAttachmentSpecification>(&specification);
			auto attachment = CreateAttachment(specs);

			FinalizeAttachment(attachment, specs, true, 0);
		}
	}

	void Framebuffer::Attach(uint32_t attachmentPoint, Ref<Texture> attachment, uint32_t mipLevel) {
		glNamedFramebufferTexture(static_cast<IDType>(*this), attachmentPoint, static_cast<GLuint>(*attachment),
		                          static_cast<GLint>(mipLevel));

		LOG_GL_DEBUG("Framebuffer '{}': Attached texture '{}' to point {} (mip={})",
			Label(), attachment->Label(), Utils::AttachmentPointToString(attachmentPoint), mipLevel);
	}

	void Framebuffer::Attach(uint32_t attachmentPoint, Ref<Texture> attachment, uint32_t mipLevel, uint32_t layer) {
		glNamedFramebufferTextureLayer(static_cast<IDType>(*this), attachmentPoint, static_cast<GLuint>(*attachment),
		                               static_cast<GLint>(mipLevel),
		                               static_cast<GLint>(layer));

		LOG_GL_DEBUG("Framebuffer '{}': Attached layered texture '{}' to point {} (mip={}, layer={})",
			Label(), attachment->Label(), Utils::AttachmentPointToString(attachmentPoint), mipLevel, layer);
	}

	void Framebuffer::Attach(uint32_t attachmentPoint, Ref<RenderBuffer> attachment) {
		glNamedFramebufferRenderbuffer(static_cast<IDType>(*this), attachmentPoint, GL_RENDERBUFFER, *attachment);
		LOG_GL_DEBUG("Framebuffer '{}': Attached renderbuffer to point {}",
			Label(), Utils::AttachmentPointToString(attachmentPoint));
	}

	uint32_t Framebuffer::DepthAttachmentPoint(ImageFormat format) const {
		const auto& info = Utils::ToGLDepthAttachmentPoint(format);
		m_Internals->DepthBuffer = info.HasDepth;
		m_Internals->Stencil = info.HasStencil;

		return info.AttachmentPoint;
	}

	void Framebuffer::AttachDepth(ImageFormat format, Ref<Texture> attachment, uint32_t mipLevel) {
		Attach(DepthAttachmentPoint(format), attachment, mipLevel);
		m_Internals->DepthAttachment = attachment;

		LOG_GL_DEBUG("Framebuffer '{}': Attached texture '{}' as depth attachment (format={}, mip={}, layered=false)",
			Label(), attachment->Label(), format, mipLevel);
	}

	void Framebuffer::AttachDepth(ImageFormat format, Ref<Texture> attachment, uint32_t mipLevel, uint32_t layer) {
		Attach(DepthAttachmentPoint(format), attachment, mipLevel, layer);
		m_Internals->DepthAttachment = attachment;

		LOG_GL_DEBUG("Framebuffer '{}': Attached texture '{}' as depth attachment (format={}, mip={}, layered=true, layer={})",
			Label(), attachment->Label(), format, mipLevel, layer);
	}

	void Framebuffer::AttachDepth(ImageFormat format, Ref<RenderBuffer> attachment) {
		Attach(DepthAttachmentPoint(format), attachment);
		LOG_GL_DEBUG("Framebuffer '{}': Attached renderbuffer as depth attachment (format={})",
			Label(), format);
		m_Internals->DepthAttachment = attachment;
	}

	TextureSpec Framebuffer::CreateAttachmentSpec(const FramebufferSpecification fb,
	                                              const FramebufferTextureAttachmentSpecification& tex) const {
		TextureSpec spec;

		spec.Size = fb.Size;
		spec.ImageFormat = tex.Format;
		spec.Samples = fb.Samples;
		spec.Label = tex.Label;
		spec.Usage = Utils::FormatUsageMapping(tex.Format);

		return spec;
	}

	Ref<Texture> Framebuffer::CreateAttachment(const FramebufferTextureAttachmentSpecification& specs) const {
		auto texture = Texture::Create(CreateAttachmentSpec(GetSpecification(), specs));
		return texture;
	}

	Ref<RenderBuffer> Framebuffer::CreateAttachment(const FramebufferRenderBufferAttachmentSpecification& specs) const {
		const auto& specification = GetSpecification();
		return RenderBuffer::Create(specification.Size, specification.Samples, specs.Format);
	}

	void Framebuffer::FinalizeAttachment(const Ref<Texture>& texture,
	                                     const FramebufferTextureAttachmentSpecification& specs, bool isDepth,
	                                     uint32_t attachmentPoint) {
		if (!texture->IsMultisampled()) {
			if (specs.MipLevel > 0)
				texture->GenerateMipMaps();

			texture->SetWrapping(specs.WrapS, specs.WrapT);
			texture->SetFilters(specs.MinFilter, specs.MagFilter);
		}

		LOG_GL_TRACE("Finalized texture attachment '{}': size={}x{}, format={}, usage={}, multisampled={}",
			specs.Label,
			texture->Width(), texture->Height(),
			specs.Format,
			texture->GetUsage(),
			texture->IsMultisampled());

		if (isDepth) {
			if (m_Internals->Specification.Layered) {
				AttachDepth(specs.Format, texture, specs.MipLevel, specs.Layer);
			}
			else {
				AttachDepth(specs.Format, texture, specs.MipLevel);
			}

			m_Internals->DepthAttachment = texture;
		}
		else {
			if (m_Internals->Specification.Layered) {
				Attach(GL_COLOR_ATTACHMENT0 + attachmentPoint, texture, specs.MipLevel, specs.Layer);
			}
			else {
				Attach(GL_COLOR_ATTACHMENT0 + attachmentPoint, texture, specs.MipLevel);
			}

			m_Internals->ColorAttachments.emplace_back(texture);
		}
	}

	Framebuffer::Internals::Internals(const FramebufferSpecification& specification) : Specification(specification) {
		if (!specification.SwapchainTarget)
			glCreateFramebuffers(1, &ID);
		else
			ID = 0;
	}

	Framebuffer::Internals::~Internals() {
		glDeleteFramebuffers(1, &ID);
	}

	void Framebuffer::Internals::Invalidate() {
		glDeleteFramebuffers(1, &ID);
		ColorAttachmentCount = 0;
		ColorAttachments.clear();
		if (!Specification.SwapchainTarget)
			glCreateFramebuffers(1, &ID);
		else
			ID = 0;
	}

	Framebuffer::Status Framebuffer::Internals::CheckStatus() {
		return Status = Utils::GlGetStatus(ID);
	}
}
