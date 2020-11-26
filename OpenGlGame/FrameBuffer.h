#pragma once

#include <glad/glad.h>

#include "Types.h"
#include "Texture.h"

namespace Game
{
	class FrameBuffer
	{
	public:
		using IdType = uint32_t;

	private:
		Pointer<IdType> m_FrameBuffer;

		Pointer<Texture> m_ColorBuffer;
		Pointer<Texture> m_DepthBuffer;

	protected:
		explicit FrameBuffer(IdType id);
		
	public:
		FrameBuffer(uint32_t width, uint32_t height, uint8_t colorDepth = 32, uint8_t depthBuffer = 24);

		operator IdType() const { return *m_FrameBuffer; }

		const Texture& GetColorBuffer() const { return *m_ColorBuffer; }
		const Texture& GetDepthBuffer() const { return *m_DepthBuffer; }

		void Bind(int32_t x, int32_t y, int32_t width, int32_t height) const;
		void Bind(int32_t width, int32_t height) const { Bind(0, 0, width, height); }

		void Bind(Vector2i position, Vector2i size) const { Bind(position.X, position.Y, size.Width, size.Height); }
		void Bind(Vector2i size) const { Bind(0, 0, size.Width, size.Height); }
		
		void Bind() const;

		FrameBuffer* GetDefault();

		static Vector2i MaxViewportSize();
	private:
		static void DeleteFrameBuffer(IdType* id);
		static auto CreateFrameBuffer();
	};
}
