#pragma once

#include <glad/glad.h>
#include <vector>

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
		struct Internals
		{
			IdType Id = 0;
			size_t ColorAttachments = 0;

			Internals();
			explicit Internals(IdType id);
			~Internals();
		};

		Pointer<Internals> m_Internals = nullptr;
	protected:
		explicit FrameBufferObject(IdType id);

	protected:
		FrameBufferObject();

	public:
		virtual ~FrameBufferObject() {}
		operator IdType() const { return m_Internals->Id; }

		void Bind(int32_t x, int32_t y, int32_t width, int32_t height) const;
		void Bind(int32_t width, int32_t height) const { Bind(0, 0, width, height); }

		void Bind(Vector2i position, Vector2i size) const { Bind(position.X, position.Y, size.Width, size.Height); }
		void Bind(Vector2i size) const { Bind(0, 0, size.Width, size.Height); }

		void Bind() const;

		size_t GetNumColorAttachments() const { return m_Internals->ColorAttachments; }
		
		[[nodiscard]] Status GetStatus() const;

		static FrameBufferObject* GetDefault();

		static Vector2i MaxViewportSize();

	protected:
		void Attach(uint32_t attachment, Pointer<Texture> texture);
		void Attach(uint32_t attachment, Pointer<RenderBuffer> renderBuffer);

		Pointer<Texture> SetUpColorTextureAttachment(uint32_t width, uint32_t height, uint32_t index, uint8_t depth);
		Pointer<RenderBuffer> SetUpColorRenderBufferAttachment(uint32_t width, uint32_t height, uint32_t indext, uint8_t depth);

		Pointer<Texture> SetUpDepthTextureAttachment(uint32_t width, uint32_t height, uint8_t depth, bool stencil = false);
		Pointer<RenderBuffer> SetUpDepthRenderBufferAttachment(uint32_t width, uint32_t height, uint8_t depth, bool stencil = false);

		void CheckCompletion() const;
	};

	template <class ColorBufferType, class DepthBufferType>
	class FrameBuffer : public FrameBufferObject
	{
		std::vector<Pointer<ColorBufferType>> m_ColorBuffer;
		Pointer<DepthBufferType> m_DepthBuffer = nullptr;

	public:
		FrameBuffer(
			uint32_t width,
			uint32_t height,
			uint32_t nrColorAttachemnts = 1,
			uint8_t colorDepth = 32,
			uint8_t depthBuffer = 24,
			bool stencil        = true
			) : FrameBufferObject()
		{
			for(uint32_t i = 0; i < nrColorAttachemnts; ++i)
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

				if constexpr(std::is_same_v<DepthBufferType, Texture>)
					m_DepthBuffer = SetUpDepthTextureAttachment(width, height, depthBuffer, stencil);
				if constexpr(std::is_same_v<DepthBufferType, RenderBuffer>)
					m_DepthBuffer = SetUpDepthRenderBufferAttachment(width, height, depthBuffer, stencil);
				else
				{
					ASSERT(false, "Unkown color Buffer type for framebuffer");
					break;
				}
			}
			CheckCompletion();
		}

		Pointer<ColorBufferType> GetColorAttachment(uint32_t index) const
		{
			ASSERT(GetNumColorAttachments() > index, "Out of range");

			if (GetNumColorAttachments() <= index)
				throw std::out_of_range("Out of range");

			return m_ColorBuffer[index];
		}

		const std::vector<Pointer<ColorBufferType>> &GetColorAttachments() const { return m_ColorBuffer; }
		std::vector<Pointer<ColorBufferType>> GetColorAttachments() { return m_ColorBuffer; }

		Pointer<ColorBufferType> GetColorBuffer(uint32_t index = 0) const { return m_ColorBuffer.at(index); }
		Pointer<DepthBufferType> GetDepthBuffer() const { return m_DepthBuffer; }
	};
}
