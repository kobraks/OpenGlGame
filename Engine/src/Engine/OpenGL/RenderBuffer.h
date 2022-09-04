#pragma once

#include "Engine/Core/Base.h"
#include "Engine/OpenGL/GLEnums.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

namespace Game
{
	class RenderBuffer
	{
	public:
		using IDType = uint32_t;

	private:

		class Internals
		{
			bool m_Provided = false;
		public:
			IDType ID = 0;

			OpenGlFunctions Functions;

			Vector2u Size;

			InternalFormat Format = InternalFormat::RGBA8;
			uint32_t Samples = 1;

			Internals();
			explicit Internals(IDType id);
			~Internals();
		};

		Pointer<Internals> m_Internals;

	protected:
		explicit RenderBuffer(IDType id);
	public:
		RenderBuffer();
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples = 0);
		RenderBuffer(const Vector2u &size, InternalFormat format, uint32_t samples = 0);

		operator IDType () const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void Bind() const;

		void Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples = 0);
		void Storage(const Vector2u &size, InternalFormat format, uint32_t samples = 0);

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		uint32_t Samples() const { return m_Internals->Samples; }
		InternalFormat Format() const { return m_Internals->Format; }

		static RenderBuffer *GetDefault();
		static uint32_t MaxSamples();
	};
}
