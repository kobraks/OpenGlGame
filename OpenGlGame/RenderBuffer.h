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
		class Internals
		{
			bool m_IdProvided = false;
		public:
			IdType Id = 0;

			Vector2u Size = {};

			InternalFormat Format = InternalFormat::RGBA8;
			uint32_t Samples = 1;

			Internals();
			explicit Internals(IdType id);
			~Internals();
		};

		Pointer<Internals> m_Internals;
	protected:
		explicit RenderBuffer(IdType id);
	public:
		RenderBuffer();
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format);
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples);

		operator IdType() const { return m_Internals->Id; }
		IdType Id() const { return m_Internals->Id; };

		void Bind() const;

		void Storage(uint32_t width, uint32_t height, InternalFormat format);
		void Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples);
		
		void Storage(const Vector2u &size, InternalFormat format);
		void Storage(const Vector2u &size, InternalFormat format, uint32_t samples);

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		uint32_t Samples() const { return m_Internals->Samples; }

		InternalFormat Format() const { return m_Internals->Format; }

		static RenderBuffer* GetDefault();
		static uint32_t MaxSamples();
	};
}
