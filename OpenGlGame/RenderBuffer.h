#pragma once

#include "Types.h"

#include "Texture.h"

namespace Game
{
	class RenderBuffer
	{
	public:
		using IdType = uint32_t;
	private:
		Pointer<IdType> m_RenderBuffer;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		InternalFormat m_Format = InternalFormat::RGBA;
		uint32_t m_Samples = 1;
	protected:
		explicit RenderBuffer(IdType id);
	public:
		RenderBuffer();
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format);
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples);

		operator IdType() const { return *m_RenderBuffer; }
		IdType ID() const { return *this; };

		void Bind() const;

		void Storage(uint32_t width, uint32_t height, InternalFormat format);
		void Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples);

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }

		uint32_t Samples() const { return m_Samples; }

		InternalFormat Format() const { return m_Format; }
		
		Vector2u Size() const { return Vector2u(m_Width, m_Height); }

		static RenderBuffer* GetDefault();

		static uint32_t MaxSamples();
	};
}
