#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"

namespace Engine {
	class Image;

	enum class Wrapping {
		Repeat = 0,
		ClampEdge,
		ClampBorder,
		MirroredRepeat,
	};

	enum class Filter {
		Nearest = 0,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	class Texture {
		friend class FramebufferObject;
		template <class ColorAttachmentType, class DepthAttachmentType>
		friend class Framebuffer;

	public:
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

		using IDType = uint32_t;

		~Texture() = default;

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void GenerateMipMaps() const;
		bool HasMipMapsGenerated() const { return m_Internals->MipMapGenerated; }
		bool IsMultisampled() const { return m_Internals->Multisampled; }

		void Bind() const;
		void BindUnit(uint32_t sampler = 0) const;

		static Ref<Texture> Create(const Vector2u& size, InternalFormat internalFormat = InternalFormat::RGBA8,
		                             const uint8_t* pixels = nullptr, DataType dataType = DataType::UnsignedByte,
		                             DataFormat dataFormat = DataFormat::RGBA);
		static Ref<Texture> Create(const Vector2u& size, uint32_t samples,
		                             InternalFormat internalFormat = InternalFormat::RGBA8,
		                             const uint8_t* pixels = nullptr, DataType dataType = DataType::UnsignedByte,
		                             DataFormat dataFormat = DataFormat::RGBA);

		static Ref<Texture> Create(Ref<Image> image);

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter filter);
		void SetMagFilter(Filter filter);

		Filter GetMagFilter() const { return m_Internals->Filter.Mag; }
		Filter GetMinFilter() const { return m_Internals->Filter.Min; }

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		uint32_t Samples() const { return m_Internals->Samples; }

		Ref<Image> ToImage() const;
		Ref<Image> GetImage(const Vector2u& size, const Vector2i& offset) const;

		void Clear(int value = 0);
		void Clear(void* pixels = nullptr, DataFormat dataFormat = DataFormat::RGBA, DataType dataType = DataType::Int);
		void Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

		void GetPixels(void* pixels, uint32_t size) const;

		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture& texture);
		void Update(const Texture& texture, const Vector2i& offset);

		void Update(Ref<Image> image);
		void Update(Ref<Image> image, const Vector2i& offset);

		void Swap(Texture& to);

		static Vector2u GetMaxDim() { return {GetMaxSize(), GetMaxSize()}; }
		static uint32_t GetMaxSize();

		bool operator==(const Texture& texture) const {
			return m_Internals->ID == texture.m_Internals->ID;
		}

		void Resize(const Vector2u& size);
	protected:
		Texture(bool multisampled = false);

		static bool CheckSize(const Vector2u& size);

		void CreateTexture(uint32_t samples, const Vector2u& size, InternalFormat internalFormat, const void* pixels = nullptr, DataType dataType = DataType::UnsignedByte, DataFormat dataFormat = DataFormat::RGBA);

		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat, DataType dataType);

	private:
		struct InternalWrapping {
			Wrapping S = Wrapping::Repeat;
			Wrapping T = Wrapping::Repeat;
		};

		struct InternalFilter {
			Filter Mag = Filter::Linear;
			Filter Min = Filter::NearestMipmapLinear;
		};

		struct Internals {
			IDType ID;
			Vector2u Size;

			bool MipMapGenerated = false;
			bool Multisampled = false;

			uint32_t Samples = 1;

			InternalWrapping Wrapping;
			InternalFilter Filter;
			InternalFormat InternalFormat;

			Internals(bool multisampled = false);
			~Internals();

			void Bind() const;
			void BindUnit(uint32_t sampler) const;

			void Allocate(const Vector2u& size, enum InternalFormat internalFormat);
			void Allocate(const Vector2u& size, uint32_t samples, enum InternalFormat internalFormat);

			void SendImage(const void* pixels, const Vector2u& size, const Vector2i& offset = {0, 0}, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);

			void GetImage(void* pixels, uint32_t size) const;
			void GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset = {0, 0});

			void Clear(void* pixels, DataFormat dataFormat, DataType dataType);
			void Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

			void CheckSubRegionSize(const Vector2i& offset, const Vector2u& size) const;

			void SetParameter(uint32_t name, int parameter);
			void GetParameter(uint32_t name, int* parameter) const;

			void Invalidate();
		};

		Ref<Internals> m_Internals;
	};
}
