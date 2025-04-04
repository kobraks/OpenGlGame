#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include <vector>
#include <initializer_list>

namespace Engine {
	class Texture;
	class RenderBuffer;

	enum class FramebufferAttachmentFormat {
		None = 0,

		RGBA8,
		RedInteger,

		DepthComponent,
		DepthComponent16,
		DepthComponent24,
		DepthComponent32,
		Depth24Stencil8,
		Depth32FStencil8,

		Depth = Depth24Stencil8
	};

	struct FramebufferAttachmentSpecification {
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferAttachmentFormat> attachments) : Attachments(attachments) {}

		std::vector<FramebufferAttachmentFormat> Attachments;
	};

	struct FramebufferSpecification {
		Vector2u Size;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
	};

	class FramebufferObject {
	public:
		using IDType = uint32_t;

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

		virtual ~FramebufferObject() = default;
		operator IDType() const { return m_Internals->Id; }

		void Bind(const Vector2i& position, const Vector2u& size) const;
		void Bind(const Vector2u& size) const { Bind({ 0, 0 }, size); }
		void Bind() const;

		void Unbind() const;

		uint32_t GetColorAttachmentCount() const { return m_Internals->ColorAttachmentCount; }

		int ReadPixel(uint32_t attachmentIndex, const Vector2i& position) const;

		const Vector2u& GetSize() const { return m_Internals->Specification.Size; }
		uint32_t Width() const { return m_Internals->Specification.Size.Width; }
		uint32_t Height() const { return m_Internals->Specification.Size.Height; }

		virtual void Invalidate();
		void Resize(const Vector2u& size);

		[[nodiscard]] Status GetStatus() const;

		static Vector2u MaxViewportSize();

		uint32_t SamplesCount() const { return m_Internals->Specification.Samples; }

		bool HasStencilTest() const { return m_Internals->Stencil; }
		bool HasDepthBuffer() const { return m_Internals->DepthBuffer; }
		bool HasColorAttachment() const { return m_Internals->ColorAttachmentCount > 0; }

		FramebufferSpecification GetSpecification() const { return m_Internals->Specification; }
	private:
		struct Internals {
			IDType Id = 0;
			uint32_t ColorAttachmentCount = 0;
			FramebufferSpecification Specification;

			bool Stencil = false;
			bool DepthBuffer = false;

			Internals(const FramebufferSpecification& specification);
			~Internals();

			void Invalidate();
		};

	protected:
		FramebufferObject(const FramebufferSpecification& specification);

		void Attach(uint32_t index, Ref<Texture> texture);
		void Attach(uint32_t attachment, Ref<RenderBuffer> renderBuffer);

		Ref<Texture> CreateColorTextureAttachment(FramebufferAttachmentFormat format);
		Ref<RenderBuffer> CreateColorRenderBufferAttachment(FramebufferAttachmentFormat format);

		Ref<Texture> CreateDepthTextureAttachment(FramebufferAttachmentFormat format);
		Ref<RenderBuffer> CreateDepthRenderBufferAttachment(FramebufferAttachmentFormat format);

		void CheckCompleteness() const;

		void SetUpBuffers(uint32_t num);
	private:

		Ref<Internals> m_Internals = nullptr;
	};

	template<class ColorBufferType = Texture, class DepthBufferType = Texture>
	class Framebuffer : public FramebufferObject {
	public:
		static Ref<Framebuffer<ColorBufferType, DepthBufferType>> Create(const FramebufferSpecification& specification);

		void Invalidate() override;

		Ref<ColorBufferType> GetColorAttachment(uint32_t index = 0) const;

		const std::vector<Ref<ColorBufferType>>& GetColorAttachments() const { return m_ColorAttachments; }
		std::vector<Ref<ColorBufferType>>& GetColorAttachments() { return m_ColorAttachments; }

		Ref<DepthBufferType> GetDepthBuffer() const { return m_DepthBuffer;  }

	protected:
		Framebuffer(const FramebufferSpecification& specification) : FramebufferObject(specification) {}

		void SetUpAttachments();
	private:
		
		std::vector<Ref<ColorBufferType>> m_ColorAttachments;
		Ref<DepthBufferType> m_DepthBuffer = nullptr;
	};

	template <class ColorBufferType, class DepthBufferType>
	Ref<Framebuffer<ColorBufferType, DepthBufferType>> Framebuffer<ColorBufferType, DepthBufferType>::Create(
		const FramebufferSpecification& specification) {
		auto framebuffer = Ref<Framebuffer<ColorBufferType, DepthBufferType>>(new Framebuffer<ColorBufferType, DepthBufferType>(specification));

		framebuffer->SetUpAttachments();

		return framebuffer;
	}

	template <class ColorBufferType, class DepthBufferType>
	void Framebuffer<ColorBufferType, DepthBufferType>::Invalidate() {
		FramebufferObject::Invalidate();
		m_DepthBuffer = nullptr;
		m_ColorAttachments.clear();

		FramebufferObject::Invalidate();

		SetUpAttachments();
		SetUpBuffers(m_ColorAttachments.size());
	}

	template <class ColorBufferType, class DepthBufferType>
	Ref<ColorBufferType> Framebuffer<ColorBufferType, DepthBufferType>::GetColorAttachment(uint32_t index) const {
		ENGINE_ASSERT(index < m_ColorAttachments.size());

		if (index < m_ColorAttachments.size())
			return m_ColorAttachments[index];

		throw std::out_of_range("Out of range");
	}

	template <class ColorBufferType, class DepthBufferType>
	void Framebuffer<ColorBufferType, DepthBufferType>::SetUpAttachments() {
		const auto& attachmentFormat = GetSpecification().Attachments.Attachments;

		for (const auto& format : attachmentFormat) {
			if (format > FramebufferAttachmentFormat::RedInteger) {
				if constexpr (!std::is_same_v<DepthBufferType, nullptr_t>) {
					if constexpr (std::is_same_v<DepthBufferType, Texture>)
						m_DepthBuffer = CreateDepthTextureAttachment(format);
					else if constexpr (std::is_same_v<DepthBufferType, RenderBuffer>)
						m_DepthBuffer = CreateDepthRenderBufferAttachment(format);
					else
						ENGINE_ASSERT(false);
				}
			}
			else {
				if constexpr (!std::is_same_v<DepthBufferType, nullptr_t>) {
					if constexpr (std::is_same_v<DepthBufferType, Texture>)
						m_ColorAttachments.emplace_back(CreateColorTextureAttachment(format));
					else if constexpr (std::is_same_v<DepthBufferType, RenderBuffer>)
						m_ColorAttachments.emplace_back(CreateColorRenderBufferAttachment(format));
					else
						ENGINE_ASSERT(false);
				}
			}
		}
	}
}
