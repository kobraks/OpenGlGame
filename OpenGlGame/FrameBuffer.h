#pragma once

#include <glad/glad.h>

#include "Types.h"
#include "Assert.h"

namespace Game
{
	class Texture;
	class RenderBuffer;
	
	class FrameBufferObject
	{
	public:
		using IdType = uint32_t;

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
		Pointer<IdType> m_FrameBuffer = nullptr;

	protected:
		explicit FrameBufferObject(IdType id);

	protected:
		FrameBufferObject();
		
	public:
		virtual ~FrameBufferObject() {}
		operator IdType() const { return *m_FrameBuffer; }

		void Bind(int32_t x, int32_t y, int32_t width, int32_t height) const;
		void Bind(int32_t width, int32_t height) const { Bind(0, 0, width, height); }

		void Bind(Vector2i position, Vector2i size) const { Bind(position.X, position.Y, size.Width, size.Height); }
		void Bind(Vector2i size) const { Bind(0, 0, size.Width, size.Height); }
		
		void Bind() const;

		[[nodiscard]] Status GetStatus() const;
		
		static FrameBufferObject* GetDefault();

		static Vector2i MaxViewportSize();

	protected:
		void Attach(uint32_t attachment, Pointer<Texture> texture);
		void Attach(uint32_t attachment, Pointer<RenderBuffer> renderBuffer);
		
		Pointer<Texture> SetUpColorTextureAttachment(uint32_t width, uint32_t height, uint8_t depth);
		Pointer<RenderBuffer> SetUpColorRenderBufferAttachment(uint32_t width, uint32_t height, uint8_t depth);
	
		Pointer<Texture>  SetUpDepthTextureAttachment(uint32_t width, uint32_t height, uint8_t depth);
		Pointer<RenderBuffer> SetUpDepthRenderBufferAttachment(uint32_t width, uint32_t height, uint8_t depth);

		void CheckCompletion() const;
	private:
		
		static void DeleteFrameBuffer(IdType* id);
		static Pointer<IdType> CreateFrameBuffer();
	};

	template<class ColorBufferType, class DepthBufferType>
	class FrameBuffer : FrameBufferObject
	{
		Pointer<ColorBufferType> m_ColorBuffer = nullptr;
		Pointer<DepthBufferType> m_DepthBuffer = nullptr;

	public:
		FrameBuffer(uint32_t width, uint32_t height, uint8_t colorDepth = 32, uint8_t depthBuffer = 24) : FrameBufferObject()
		{
			if constexpr (std::is_same_v<ColorBufferType, Texture>)
				m_ColorBuffer = SetUpColorTextureAttachment(width, height, colorDepth);
			else if constexpr (std::is_same_v<ColorBufferType, RenderBuffer>)
				m_ColorBuffer = SetUpColorRenderBufferAttachment(width, height, colorDepth);
			else
			{
				ASSERT(false, "Unkown color Buffer type for framebuffer");
			}

			if constexpr (std::is_same_v<DepthBufferType, Texture>)
				m_DepthBuffer = SetUpDepthTextureAttachment(width, height, depthBuffer);
			if constexpr (std::is_same_v<DepthBufferType, RenderBuffer>)
				m_DepthBuffer = SetUpDepthRenderBufferAttachment(width, height, depthBuffer);
			else
			{
				ASSERT(false, "Unkown color Buffer type for framebuffer");
			}

			CheckCompletion();
		}

		const ColorBufferType& GetColorBuffer() const { return *m_ColorBuffer; }
		const DepthBufferType& GetDepthBuffer() const { return *m_DepthBuffer; }
		
		ColorBufferType& GetColorBuffer() { return *m_ColorBuffer; }
		DepthBufferType& GetDepthBuffer() { return *m_DepthBuffer; }
	};
}
