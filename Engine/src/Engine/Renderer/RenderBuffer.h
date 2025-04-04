#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

namespace Engine {
	class RenderBuffer {
	public:
		using IDType = uint32_t;

		enum class DataFormat : uint32_t {
			Red,
			RG,
			RGB,
			BGR,
			RGBA,
			BGRA,

			RedInteger,
			RGInteger,
			RGBInteger,
			BGRInteger,
			RGBAInteger,
			BGRAInteger,

			StencilIndex,
			DepthComponent,
			DepthStencil
		};

		enum class InternalFormat : uint32_t {
			CompressedRed, CompressedRedRGTC1, CompressedRG, CompressedRGB, CompressedRGBA, CompressedRGRGTC2,
			CompressedSignedRedRGTC1, CompressedSignedRGRGTC2, CompressedSRGB,
			DepthStencil, Depth24Stencil8, Depth32FStencil8, DepthComponent, DepthComponent16, DepthComponent24, DepthComponent32,
			DepthComponent32F,
			R16F, R16I, R16SNorm, R16UI, R32F, R32I, R32UI, R3G3B2, R8, R8I, R8SNorm, R8UI,
			Red, RG, RG16, RG16F, RG16SNorm, RG32F, RG32I, RG32UI, RG8, RG8I, RG8SNorm, RG8UI,
			RGB, RGB10, RGB10A2, RGB12, RGB16, RGB16F, RGB16I, RGB16UI,
			RGB32F, RGB32I, RGB32UI, RGB4, RGB5, RGB5A1, RGB8, RGB8I, RGB8UI, RGB9E5, RGBA,
			RGBA12, RGBA16, RGBA16F, RGBA16I, RGBA16UI, RGBA2, RGBA32F, RGBA32I, RGBA32UI, RGBA4, RGBA8,
			RGBA8UI, SRGB8, SRGB8A8, SRGBA
		};

		enum class DataType : uint32_t {
			Byte, UnsignedByte,
			Short, UnsignedShort,
			Int, UnsignedInt,
			Float, Double
		};

		static Ref<RenderBuffer> Create(const Vector2u& size, uint32_t samples, InternalFormat internalFormat);


		IDType ID() const { return m_Internals->ID; }
		operator IDType() const { return m_Internals->ID; }

		void Bind() const;
		void Unbind() const;

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		uint32_t Samples() const { return m_Internals->Samples; }
		bool IsMultisampled() const { return m_Internals->Multisampled; }

		InternalFormat GetInternalFormat() const { return m_Internals->InternalFormat; }
	protected:
		RenderBuffer(bool multisampled = false);

	private:
		void CreateBuffer(uint32_t samples, const Vector2u& size, enum InternalFormat internalFormat);

		struct Internals {
			IDType ID;
			Vector2u Size;

			uint32_t Samples = 1;
			bool Multisampled = false;

			InternalFormat InternalFormat;

			Internals(bool multisampled);
			~Internals();

			void Allocate(uint32_t samples, const Vector2u& size, enum InternalFormat internalFormat);
			void Allocate(const Vector2u& size, enum InternalFormat internalFormat);
		};

		Ref<Internals> m_Internals;
	};

	inline void RenderBuffer::CreateBuffer(uint32_t samples, const Vector2u& size, enum InternalFormat internalFormat) {
		if (samples > 1) {
			m_Internals->Allocate(samples, size, internalFormat);
		} else {
			m_Internals->Allocate(size, internalFormat);
		}
	}
}
