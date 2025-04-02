#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

namespace Engine {
	class Texture2D;
	class RenderBuffer;

	class FramebufferObject {
	public:
		using IdType = uint32_t;

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
		operator IdType() const { return m_Internals->Id; }

		void Bind(const Vector2i& position, const Vector2u& size) const;
		void Bind(const Vector2u& size) const { Bind({ 0, 0 }, size); }
		void Bind() const;

		void Unbind() const;

		uint32_t GetColorAttachmentCount() const { return m_Internals->ColorAttachmentCount; }

		int ReadPixel(uint32_t attachmentIndex, const Vector2i& position);

		const Vector2u& GetSize() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		virtual void Invalidate();
		void Resize(const Vector2u& size);

		[[nodiscard]] Status GetStatus() const;

		static Vector2u MaxViewportSize();

		uint32_t SamplesCount() const { return m_Internals->Samples; }

		bool HasStencilTest() const { return m_Internals->Stencil; }

		bool HasDepthBuffer() const { return m_Internals->DepthBufferDepth > 0; }

		bool HasColorAttachment() const { return m_Internals->ColorDepth > 0; }

	private:
		struct Internals {
			IdType Id = 0;
			uint32_t ColorAttachmentCount = 0;
			Vector2u Size;

			uint32_t Samples = 1;
			uint8_t DepthBufferDepth = 0;
			uint8_t ColorDepth = 0;

			bool Stencil = true;

			Internals();
			~Internals();

			void Invalidate();
		};

	protected:
		FramebufferObject(const Vector2u& size);

		void Attach(uint32_t index, Ref<Texture2D> texture);
		void Attach(uint32_t attachment, Ref<RenderBuffer> renderBuffer);

		Ref<Texture2D> CreateColorTextureAttachment(const Vector2u& size, uint32_t index, uint8_t depth, uint32_t samples = 1);
		Ref<RenderBuffer> CreateColorRenderBufferAttachment(const Vector2u& size, uint32_t index, uint8_t depth, uint32_t samples = 1);

		Ref<Texture2D> CreateDepthTextureAttachment(const Vector2u& size, uint8_t depth, bool stencil = false, uint32_t samples = 1);
		Ref<RenderBuffer> CreateDepthRenderBufferAttachment(const Vector2u& size, uint8_t depth, bool stencil = false, uint32_t samples = 1);

		void CheckCompleteness() const;

		uint8_t DepthBufferDepth() const { return m_Internals->DepthBufferDepth; }
		uint8_t ColorDepth() const { return m_Internals->ColorDepth; }
	private:

		Ref<Internals> m_Internals = nullptr;
	};

	template<class ColorBufferType = Texture2D, class DepthBufferType = Texture2D>
	class Framebuffer : public FramebufferObject {
	public:
		static Ref<Framebuffer<ColorBufferType, DepthBufferType>> Create(const Vector2u& size, uint32_t colorAttachmentCount = 1, uint32_t samples = 1, uint8_t colorDepth = 32, uint8_t depthBufferDepth = 24, bool stencil = true) {
			auto framebuffer = Ref<Framebuffer<ColorBufferType, DepthBufferType>>(new Framebuffer<ColorBufferType, DepthBufferType>(size));

			if constexpr (!std::is_same_v<ColorBufferType, nullptr_t>)
				framebuffer->SetUpColorAttachments(size, colorAttachmentCount, colorDepth, samples);

			if constexpr (!std::is_same_v<DepthBufferType, nullptr_t>)
				framebuffer->SetUpDepthAttachment(size, depthBufferDepth, samples, stencil);

			framebuffer->CheckCompleteness();

			return framebuffer;
		}

		void Invalidate() override {

			if constexpr (!std::is_same_v<ColorBufferType, nullptr_t>) {
				m_ColorAttachments.clear();
			}

			if constexpr (!std::is_same_v<DepthBufferType, nullptr_t>)
				m_DepthBuffer = nullptr;

			FramebufferObject::Invalidate();

			if constexpr (!std::is_same_v<ColorBufferType, nullptr_t>) {
				SetUpColorAttachments(GetSize(), GetColorAttachmentCount(), ColorDepth(), SamplesCount());
			}

			if constexpr (!std::is_same_v<DepthBufferType, nullptr_t>) {
				SetUpDepthAttachment(GetSize(), DepthBufferDepth(), SamplesCount(), HasStencilTest());
			}
		}

		Ref<ColorBufferType> GetColorAttachment(uint32_t index = 0) const {
			ENGINE_ASSERT(index < GetColorAttachmentCount());

			if (GetColorAttachmentCount() <= index)
				throw std::out_of_range("Out of range");

			return m_ColorAttachments[index];
		}

		const std::vector<Ref<ColorBufferType>>& GetColorAttachments() const { return m_ColorAttachments; }
		std::vector<Ref<ColorBufferType>>& GetColorAttachments() { return m_ColorAttachments; }

		Ref<DepthBufferType> GetDepthBuffer() const { return m_DepthBuffer;  }

	protected:
		Framebuffer(const Vector2u& size) : FramebufferObject(size) {};

		void SetUpColorAttachments(const Vector2u& size, uint32_t count, uint8_t depth, uint32_t samples) {
			for (uint32_t i = 0; i < count; ++i) {
				if constexpr (std::is_same_v<ColorBufferType, Texture2D>)
					m_ColorAttachments.emplace_back(CreateColorTextureAttachment(size, i, depth, samples));
				else if constexpr (std::is_same_v<ColorBufferType, RenderBuffer>)
					m_ColorAttachments.emplace_back(CreateColorRenderBufferAttachment(size, i, depth, samples));
				else {
					ENGINE_ASSERT(false);
					break;
				}
			}
		}

		void SetUpDepthAttachment(const Vector2u& size, uint8_t depth, uint32_t samples, bool stencil = true) {
			if constexpr (std::is_same_v<ColorBufferType, Texture2D>)
				m_DepthBuffer = CreateDepthTextureAttachment(size, depth, stencil);
			else if constexpr (std::is_same_v<DepthBufferType, RenderBuffer>)
				m_DepthBuffer = CreateDepthRenderBufferAttachment(size, depth, stencil);
			else {
				ENGINE_ASSERT(false);
			}
		}

	private:
		
		std::vector<Ref<ColorBufferType>> m_ColorAttachments;
		Ref<DepthBufferType> m_DepthBuffer = nullptr;
	};
}
