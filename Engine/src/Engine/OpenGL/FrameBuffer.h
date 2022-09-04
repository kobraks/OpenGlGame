#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/OpenGL/GLEnums.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

namespace Game
{
	class Texture;
	class RenderBuffer;

	class FrameBufferObject
	{
	public:
		using IDType = uint32_t;

		enum class Status : GLenum
		{
			Complete = GL_FRAMEBUFFER_COMPLETE,
			IncompleteAttachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
			MissingAttachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
			IncompleteDrawBuffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
			IncompleteReadBuffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
			Unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
			IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
			IncompleteLayerTargets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
		};

	private:
		struct Internals
		{
			bool m_Provided = false;

		public:
			IDType ID               = 0;
			size_t ColorAttachments = 0;

			OpenGlFunctions Functions;

			Internals();
			explicit Internals(IDType id);
			~Internals();
		};

		Pointer<Internals> m_Internals;

	protected:
		explicit FrameBufferObject(IDType id);

		FrameBufferObject();

	public:
		virtual ~FrameBufferObject() {}

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void Bind(int32_t x, int32_t y, uint32_t width, uint32_t height) const;
		void Bind(uint32_t width, uint32_t height) const { Bind(0, 0, width, height); }

		void Bind(const Vector2i &position, Vector2u size) const;
		void Bind(const Vector2u size) const { Bind({0, 0}, size); }

		void Bind() const;

		size_t GetNumColorAttachments() const { return m_Internals->ColorAttachments; }
		[[nodiscard]] Status GetStatus() const;

		static FrameBufferObject* GetDefault();
		static Vector2u MaxViewportSize();

	protected:
		void Attach(uint32_t attachment, Ref<Texture> texture);
		void Attach(uint32_t attachment, Ref<RenderBuffer> buffer);

		Ref<Texture> SetUpColorTextureAttachment(uint32_t width, uint32_t height, uint32_t index, uint8_t depth);
		Ref<Texture> SetUpColorTextureAttachment(const Vector2u &size, uint32_t index, uint8_t depth);

		Ref<RenderBuffer> SetUpColorRenderBufferAttachment(
			uint32_t width,
			uint32_t height,
			uint32_t index,
			uint8_t depth
			);
		Ref<RenderBuffer> SetUpColorRenderBufferAttachment(const Vector2u &size, uint32_t index, uint8_t depth);

		Ref<Texture> SetUpDepthTextureAttachment(uint32_t width, uint32_t height, uint8_t depth, bool stencil = false);
		Ref<Texture> SetUpDepthTextureAttachment(const Vector2u &size, uint8_t depth, bool stencil = false);

		Ref<RenderBuffer> SetUpDepthRenderBufferAttachment(
			uint32_t width,
			uint32_t height,
			uint8_t depth,
			bool stencil = false
			);
		Ref<RenderBuffer> SetUpDepthRenderBufferAttachment(const Vector2u &size, uint8_t depth, bool stencil = false);

		void CheckCompletion() const;
	};

	template <class ColorBufferType, class DepthBufferType>
	class FrameBuffer: public FrameBufferObject
	{
		std::vector<Ref<ColorBufferType>> m_ColorBuffer;
		Ref<DepthBufferType> m_DepthBuffer = nullptr;

	public:
		FrameBuffer(
			uint32_t width,
			uint32_t height,
			uint32_t colorAttachments = 1,
			uint8_t colorDepth        = 32,
			uint8_t depthBuffer       = 24,
			bool stencil              = false
			);
		explicit FrameBuffer(
			const Vector2u &size,
			uint32_t colorAttachments = 1,
			uint8_t colorDepth        = 32,
			uint8_t depthBuffer       = 24,
			bool stencil              = false
			);

		Ref<ColorBufferType> GetColorAttachment(uint32_t index) const
		{
			ASSERT(index < GetNumColorAttachments(), "Out of Range");

			if(index >= GetNumColorAttachments())
				throw std::out_of_range("Out of range");

			return m_ColorBuffer[index];
		}

		const std::vector<Pointer<ColorBufferType>>& GetColorAttachments() const { return m_ColorBuffer; }
		std::vector<Pointer<ColorBufferType>> GetColorAttachments() { return m_ColorBuffer; }

		Pointer<ColorBufferType> GetColorBuffer(uint32_t index = 0) const { return m_ColorBuffer.at(index); }
		Pointer<DepthBufferType> GetDepthBuffer() const { return m_DepthBuffer; }
	};

	template <class ColorBufferType, class DepthBufferType>
	FrameBuffer<ColorBufferType, DepthBufferType>::FrameBuffer(
		uint32_t width,
		uint32_t height,
		uint32_t colorAttachments,
		uint8_t colorDepth,
		uint8_t depthBuffer,
		bool stencil
		)
	{
		for(uint32_t i = 0; i < colorAttachments; ++i)
		{
			if constexpr(std::is_same_v<ColorBufferType, Texture>)
				m_ColorBuffer.emplace_back(SetUpColorTextureAttachment(width, height, i, colorDepth));
			else if constexpr(std::is_same_v<ColorBufferType, RenderBuffer>)
				m_ColorBuffer.emplace_back(SetUpColorRenderBufferAttachment(width, height, i, colorDepth));
			else
			{
				ASSERT(false, "Unkown color Buffer type for framebuffer");
				break;
			}
		}
		if constexpr(std::is_same_v<DepthBufferType, Texture>)
			m_DepthBuffer = SetUpDepthTextureAttachment(width, height, depthBuffer, stencil);
		if constexpr(std::is_same_v<DepthBufferType, RenderBuffer>)
			m_DepthBuffer = SetUpDepthRenderBufferAttachment(width, height, depthBuffer, stencil);
		else
		{
			ASSERT(false, "Unkown color Buffer type for framebuffer");
		}

		CheckCompletion();
	}

	template <class ColorBufferType, class DepthBufferType>
	FrameBuffer<ColorBufferType, DepthBufferType>::FrameBuffer(
		const Vector2u &size,
		uint32_t colorAttachments,
		uint8_t colorDepth,
		uint8_t depthBuffer,
		bool stencil
		)
	{
		for(uint32_t i = 0; i < colorAttachments; ++i)
		{
			if constexpr(std::is_same_v<ColorBufferType, Texture>)
				m_ColorBuffer.emplace_back(SetUpColorTextureAttachment(size, i, colorDepth));
			else if constexpr(std::is_same_v<ColorBufferType, RenderBuffer>)
				m_ColorBuffer.emplace_back(SetUpColorRenderBufferAttachment(size, i, colorDepth));
			else
			{
				ASSERT(false, "Unkown color Buffer type for framebuffer");
				break;
			}
		}
		if constexpr(std::is_same_v<DepthBufferType, Texture>)
			m_DepthBuffer = SetUpDepthTextureAttachment(size, depthBuffer, stencil);
		if constexpr(std::is_same_v<DepthBufferType, RenderBuffer>)
			m_DepthBuffer = SetUpDepthRenderBufferAttachment(size , depthBuffer, stencil);
		else
		{
			ASSERT(false, "Unkown color Buffer type for framebuffer");
		}

		CheckCompletion();
	}
}
