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

	public:
		FrameBuffer(uint32_t width, uint32_t height, uint8_t colorDepth = 32, uint8_t depthBuffer = 24);

		operator IdType() const { return *m_FrameBuffer; }

		const Texture& GetColorBuffer() const { return *m_ColorBuffer; }
		const Texture& GetDepthBuffer() const { return *m_DepthBuffer; }

		void Bind() const;
		void Bind(int32_t width, int32_t height) const;
	};
}
