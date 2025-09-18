#pragma once
#include <Engine/Core/Base.h>

#include <Engine/Core/Vector2.h>
#include <Engine/Core/Color.h>
#include "Engine/Core/Buffer.h"

#include <Engine/Renderer/FramebufferSpecification.h>

#include <variant>
#include <concepts>

namespace Engine {
	struct TextureSpec;
	class Texture;
	class RenderBuffer;

	template<typename T>
	concept ReadablePixel = std::same_as<T, int32_t> || std::same_as<T, uint32_t> || std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, Color>;

	class Framebuffer {
	public:
		using IDType = uint32_t;
		using AttachmentType = std::variant<Ref<RenderBuffer>, Ref<Texture>>;

		enum class Status : uint32_t {
			Complete = 0,
			IncompleteAttachment,
			MissingAttachment,
			IncompleteDrawBuffer,
			IncompleteReadBuffer,
			Unsupported,
			IncompleteMultisample,
			IncompleteLayerTargets,
			Undefined
		};

		static Ref<Framebuffer> Create(const FramebufferSpecification& specification);

		explicit operator IDType() const { return m_GLState->ID; }
		uint32_t RendererID() const { return m_GLState->ID;  }

		void Bind(bool adjustViewport = true) const;

		void SetViewport(const Vector2u& size) const;
		void SetViewport(const Vector2i& position, const Vector2u& size) const;

		void Unbind() const;

		uint32_t GetColorAttachmentCount() const { return m_GLState->ColorAttachmentCount; }

		// int ReadPixel(uint32_t attachmentIndex, const Vector2i& position) const;

		const Vector2u& Size() const { return m_GLState->Specification.Size;  }
		uint32_t Width() const { return m_GLState->Specification.Size.Width; }
		uint32_t Height() const { return m_GLState->Specification.Size.Height; }

		uint32_t AttachmentCount() const { return static_cast<uint32_t>(m_GLState->ColorAttachments.size()) + (m_GLState->DepthBuffer ? 1 : 0); }

		void Invalidate();
		void Resize(const Vector2u& size);

		[[nodiscard]] Status GetStatus() const;

		uint32_t SamplesCount() const { return m_GLState->Specification.Samples; }
		bool IsMultisampled() const { return m_GLState->Specification.Samples > 1; }

		bool HasStencilTest() const { return m_GLState->Stencil; }
		bool HasDepthBuffer() const { return m_GLState->DepthBuffer; }
		bool HasDepthStencil() const { return m_GLState->Stencil && m_GLState->DepthBuffer; }

		bool HasColorAttachment() const { return m_GLState->ColorAttachmentCount > 0; }

		const FramebufferSpecification& GetSpecification() const { return m_GLState->Specification; }

		void SetDrawBuffers(uint32_t drawBuffers);

		Ref<Texture> GetColorTextureAttachment(uint32_t attachmentIndex = 0) const;
		Ref<RenderBuffer> GetColorRenderBufferAttachment(uint32_t attachmentIndex = 0) const;

		AttachmentType GetColorAttachment(uint32_t attachmentIndex = 0) const;

		Ref<Texture> GetDepthTextureAttachment() const;
		Ref<RenderBuffer> GetDepthRenderBufferAttachment() const;

		AttachmentType GetDepthAttachment() const;

		std::string_view Label() const { return m_GLState->Specification.Label; }

		void Present(const Ref<Framebuffer>& source, BlitMask mask, BlitFilter filter);
		void BlitTo(const Ref<Framebuffer>& target, BlitMask mask, BlitFilter filter);

		static Vector2u MaxViewportSize();
		static uint32_t MaxColorAttachmentsCount();
		static uint32_t	MaxDrawBuffersCount();

		[[nodiscard]] Buffer ReadPixels(uint32_t attachmentIndex, const Vector2u& position, const Vector2u& size) const;

		template<ReadablePixel T>
		[[nodiscard]] T ReadPixel(uint32_t attachmentIndex, const Vector2u& position, Channel channel = Channel::Red) const {
			ENGINE_ASSERT(attachmentIndex < m_GLState->ColorAttachmentCount);
			if (attachmentIndex >= m_GLState->ColorAttachmentCount) {
				throw std::out_of_range("Attachment index out of range");
			}

			if constexpr (std::same_as<T, int32_t>) {
				return ReadPixelInt32Impl(attachmentIndex, position, channel);
			} else if constexpr (std::same_as<T, uint32_t>) {
				return ReadPixelUInt32Impl(attachmentIndex, position, channel);
			} else if constexpr (std::same_as<T, double>) {
				return ReadPixelDoubleImpl(attachmentIndex, position, channel);
			} else if constexpr (std::same_as<T, float>) {
				return ReadPixelFloatImpl(attachmentIndex, position, channel);
			} else if constexpr (std::same_as<T, Color>) {
				return ReadPixelColorImpl(attachmentIndex, position);
			}
			else {
				static_assert(false, "Unsupported pixel type");
			}
		}
	protected:
		Framebuffer(const FramebufferSpecification& specification);

		void CreateFramebuffer();

		void CheckCompleteness() const;
		void SetUpAttachments();

		void CreateColorAttachment(const FramebufferAttachmentSpecification& specification);
		void CreateDepthAttachment(const FramebufferAttachmentSpecification& specification);

		void Attach(uint32_t attachmentPoint, Ref<Texture> attachment, uint32_t mipLevel);
		void Attach(uint32_t attachmentPoint, Ref<Texture> attachment, uint32_t mipLevel, uint32_t layer);
		void Attach(uint32_t attachmentPoint, Ref<RenderBuffer> attachment);

		uint32_t DepthAttachmentPoint(ImageFormat format) const;

		void AttachDepth(ImageFormat format, Ref<Texture> attachment, uint32_t mipLevel);
		void AttachDepth(ImageFormat format, Ref<Texture> attachment, uint32_t mipLevel, uint32_t layer);
		void AttachDepth(ImageFormat format, Ref<RenderBuffer> attachment);
	private:
		TextureSpec CreateAttachmentSpec(const FramebufferSpecification fb, const FramebufferTextureAttachmentSpecification& tex) const;
		Ref<Texture> CreateAttachment(const FramebufferTextureAttachmentSpecification& specs) const;
		Ref<RenderBuffer> CreateAttachment(const FramebufferRenderBufferAttachmentSpecification& specs) const;

		void FinalizeAttachment(const Ref<Texture>& texture, const FramebufferTextureAttachmentSpecification& specs, bool isDepth, uint32_t attachmentPoint);

		int32_t ReadPixelInt32Impl(uint32_t attachmentIndex, const Vector2u& position, Channel channel) const;
		uint32_t ReadPixelUInt32Impl(uint32_t attachmentIndex, const Vector2u& position, Channel channel) const;
		float ReadPixelFloatImpl(uint32_t attachmentIndex, const Vector2u& position, Channel channel) const;
		double ReadPixelDoubleImpl(uint32_t attachmentIndex, const Vector2u& position, Channel channel) const;
		Color ReadPixelColorImpl(uint32_t attachmentIndex, const Vector2u& position) const;

		ImageFormat GetColorAttachmentFormat(uint32_t attachmentIndex) const;

		class GLState {
		public:
			IDType ID;

			uint32_t ColorAttachmentCount = 0;
			uint32_t DrawBuffers = 0;

			FramebufferSpecification Specification;

			bool Stencil = false;
			bool DepthBuffer = false;

			std::vector<AttachmentType> ColorAttachments;
			AttachmentType DepthAttachment;

			Status Status = Status::Undefined;

			GLState(const FramebufferSpecification& specification);
			~GLState();

			void Invalidate();

			enum Status CheckStatus();
		};

		Ref<GLState> m_GLState;
	};
}
