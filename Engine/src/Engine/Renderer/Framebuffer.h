#pragma once
#include <Engine/Core/Base.h>
#include <Engine/Core/Vector2.h>

#include <variant>

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

	enum class FramebufferTextureFiltering {
		Default = 0,
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	enum class FramebufferTextureWrapping {
		Default = 0,
		Repeat,
		ClampEdge,
		ClampBorder,
		MirroredRepeat,
	};

	enum class FramebufferAttachmentType {
		Texture,
		RenderBuffer,
		Default = Texture
	};

	struct FramebufferAttachmentSpecification {
		FramebufferAttachmentSpecification(FramebufferAttachmentFormat format) : Format(format) {}
		FramebufferAttachmentSpecification() = default;

		FramebufferAttachmentFormat Format;

		FramebufferTextureFiltering TextureFilterMin = FramebufferTextureFiltering::Default;
		FramebufferTextureFiltering TextureFilterMag = FramebufferTextureFiltering::Default;

		FramebufferTextureWrapping TextureWrappingS = FramebufferTextureWrapping::Default;
		FramebufferTextureWrapping TextureWrappingT = FramebufferTextureWrapping::Default;

		FramebufferAttachmentType Type = FramebufferAttachmentType::Default;
	};

	struct FramebufferSpecification {
		Vector2u Size = {0, 0};
		std::vector<FramebufferAttachmentSpecification> Attachments;
		uint32_t Samples = 1;
	};

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

		static Ref<Framebuffer> Create(const FramebufferSpecification specification);

		operator IDType() const { return m_Internals->ID; }
		uint32_t ID() const { return m_Internals->ID;  }

		void Bind(bool adjustViewport = true) const;

		void SetViewport(const Vector2u& size) const;
		void SetViewport(const Vector2i& position, const Vector2u& size) const;

		void Unbind() const;

		uint32_t GetColorAttachmentCount() const { return m_Internals->ColorAttachmentCount; }

		int ReadPixel(uint32_t attachmentIndex, const Vector2i& position) const;

		const Vector2u& Size() const { return m_Internals->Specification.Size;  }
		uint32_t Width() const { return m_Internals->Specification.Size.Width; }
		uint32_t Height() const { return m_Internals->Specification.Size.Height; }

		void Invalidate();
		void Resize(const Vector2u& size);

		[[nodiscard]] Status GetStatus() const;

		uint32_t SamplesCount() const { return m_Internals->Specification.Samples; }

		bool HasStencilTest() const { return m_Internals->Stencil; }
		bool HasDepthBuffer() const { return m_Internals->DepthBuffer; }
		bool HasColorAttachment() const { return m_Internals->ColorAttachmentCount > 0; }

		const FramebufferSpecification& GetSpecification() const { return m_Internals->Specification; }

		void SetDrawBuffers(uint32_t drawBuffers);

		Ref<Texture> GetColorTextureAttachment(uint32_t attachmentIndex = 0) const;
		Ref<RenderBuffer> GetColorRenderBufferAttachment(uint32_t attachmentIndex = 0) const;

		AttachmentType GetColorAttachment(uint32_t attachmentIndex = 0) const;

		Ref<Texture> GetDepthTextureAttachment() const;
		Ref<RenderBuffer> GetDepthRenderBufferAttachment() const;

		AttachmentType GetDepthAttachment() const;

		static Vector2u MaxViewportSize();
		static uint32_t GetMaxColorAttachments();
		static uint32_t	GetMaxDrawBuffers();
	protected:
		Framebuffer(const FramebufferSpecification& specification);

		void CheckCompleteness() const;
		void SetUpAttachments();
	private:
		class Internals {
		public:
			IDType ID;

			uint32_t ColorAttachmentCount = 0;
			uint32_t DrawBuffers = 0;

			FramebufferSpecification Specification;

			bool Stencil = false;
			bool DepthBuffer = false;

			std::vector<AttachmentType> ColorAttachments;
			AttachmentType DepthAttachment;

			Status Status;

			Internals(const FramebufferSpecification& specification);
			~Internals();

			void Invalidate();

			void CheckStatus() const;

			void Attach(uint32_t attachmentPoint, Ref<Texture> attachment);
			void Attach(uint32_t attachmentPoint, Ref<RenderBuffer> attachment);

			void CreateColorAttachment(const FramebufferAttachmentSpecification& attachmentSpecification);
			void CreateDepthAttachment(const FramebufferAttachmentSpecification& attachmentSpecification);
		private:
			uint32_t DepthAttachmentPoint(FramebufferAttachmentFormat format);
			void AttachDepth(FramebufferAttachmentFormat format, Ref<Texture> attachment);
			void AttachDepth(FramebufferAttachmentFormat format, Ref<RenderBuffer> attachment);

			Ref<Texture> CreateColorTextureAttachment(const FramebufferAttachmentSpecification& TextureSpecification) const;
			Ref<RenderBuffer> CreateColorRenderBufferAttachment(const FramebufferAttachmentSpecification& renderBufferSpecification) const;

			Ref<Texture> CreateDepthTextureAttachment(const FramebufferAttachmentSpecification& textureSpecification) const;
			Ref<RenderBuffer> CreateDepthRenderBufferAttachment(const FramebufferAttachmentSpecification& renderBufferSpecification) const;
		};

		Ref<Internals> m_Internals;
	};
}