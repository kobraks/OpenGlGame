#pragma once

#include <glad/glad.h>


#include "Color.h"
#include "Types.h"
#include "Image.h"

namespace Game
{
	enum class InternalFormat
	{
		CompressedRed = GL_COMPRESSED_RED,
		CompressedRedRGTC1 = GL_COMPRESSED_RED_RGTC1,
		CompressedRG = GL_COMPRESSED_RG,
		CompressedRGB = GL_COMPRESSED_RGB,
		CompressedRGBA = GL_COMPRESSED_RGBA,
		CompressedRGRGTC2 = GL_COMPRESSED_RG_RGTC2,
		CompressedSignedRedRGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
		CompressedSignedRGRGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
		CompressedSRGB = GL_COMPRESSED_SRGB,
		DepthStencil = GL_DEPTH_STENCIL,
		Depth24Stencil8 = GL_DEPTH24_STENCIL8,
		Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthComponent16 = GL_DEPTH_COMPONENT16,
		DepthComponent24 = GL_DEPTH_COMPONENT24,
		DepthComponent32F = GL_DEPTH_COMPONENT32F,
		R16F = GL_R16F,
		R16I = GL_R16I,
		R16SNorm = GL_R16_SNORM,
		R16UI = GL_R16UI,
		R32F = GL_R32F,
		R32I = GL_R32I,
		R32UI = GL_R32UI,
		R3G3B2 = GL_R3_G3_B2,
		R8 = GL_R8,
		R8I = GL_R8I,
		R8SNorm = GL_R8_SNORM,
		R8UI = GL_R8UI,
		Red = GL_RED,
		RG = GL_RG,
		RG16 = GL_RG16,
		RG16F = GL_RG16F,
		RG16SNorm = GL_RG16_SNORM,
		RG32F = GL_RG32F,
		RG32I = GL_RG32I,
		RG32UI = GL_RG32UI,
		RG8 = GL_RG8,
		RG8I = GL_RG8I,
		RG8SNorm = GL_RG8_SNORM,
		RG8UI = GL_RG8UI,
		RGB = GL_RGB,
		RGB10 = GL_RGB10,
		RGB10A2 = GL_RGB10_A2,
		RGB12 = GL_RGB12,
		RGB16 = GL_RGB16,
		RGB16F = GL_RGB16F,
		RGB16I = GL_RGB16I,
		RGB16UI = GL_RGB16UI,
		RGB32F = GL_RGB32F,
		RGB32I = GL_RGB32I,
		RGB32UI = GL_RGB32UI,
		RGB4 = GL_RGB4,
		RGB5 = GL_RGB5,
		RGB5A1 = GL_RGB5_A1,
		RGB8 = GL_RGB8,
		RGB8I = GL_RGB8I,
		RGB8UI = GL_RGB8UI,
		RGB9E5 = GL_RGB9_E5,
		RGBA = GL_RGBA,
		RGBA12 = GL_RGBA12,
		RGBA16 = GL_RGBA16,
		RGBA16F = GL_RGBA16F,
		RGBA16I = GL_RGBA16I,
		RGBA16UI = GL_RGBA16UI,
		RGBA2 = GL_RGBA2,
		RGBA32F = GL_RGBA32F,
		RGBA32I = GL_RGBA32I,
		RGBA32UI = GL_RGBA32UI,
		RGBA4 = GL_RGBA4,
		RGBA8 = GL_RGBA8,
		RGBA8UI = GL_RGBA8UI,
		SRGB8 = GL_SRGB8,
		SRGB8A8 = GL_SRGB8_ALPHA8,
		SRGBA = GL_SRGB_ALPHA
	};

	enum class Format
	{
		Red = GL_RED,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA
	};

	enum class DataType
	{
		Byte = GL_BYTE,
		UnsignedByte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		UnsignedShort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		UnsignedInt = GL_UNSIGNED_INT,
		Float = GL_FLOAT,
		Double = GL_DOUBLE,

		UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,
		UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
		UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
		UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5,
		UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
		UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
		UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,
		UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,
		UnsignedInt101010102 = GL_UNSIGNED_INT_10_10_10_2
	};

	enum class Wrapping
	{
		ClampEdge = GL_CLAMP_TO_EDGE,
		ClampBorder = GL_CLAMP_TO_BORDER,
		Repeat = GL_REPEAT,
		MirroredRepeat = GL_MIRRORED_REPEAT
	};

	enum class Filter
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	class Texture
	{
	public:
		using IdType = uint32_t;

	private:
		Pointer<IdType> m_Texture;

		Vector2u m_Size = {0, 0};

		InternalFormat m_InternalFormat = InternalFormat::RGBA;

		Wrapping m_S = Wrapping::Repeat;
		Wrapping m_T = Wrapping::Repeat;
		Wrapping m_R = Wrapping::Repeat;

		Filter m_Mag = Filter::Linear;
		Filter m_Min = Filter::NearestMipmapLinear;

		Color m_BorderColor = Color(0.f, 0.f, 0.f, 0.f);

		mutable bool m_MipmapGenerated = false;
	public:
		Texture();

		operator IdType() const { return *m_Texture; }
		IdType ID() const { return *m_Texture; }

		void Image2D(
			const void *data,
			DataType type,
			Format format,
			uint32_t width,
			uint32_t height,
			InternalFormat internalFormat
			);

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);
		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);

		Wrapping GetWrappingS() const { return m_S; }
		Wrapping GetWrappingT() const { return m_T; }
		Wrapping GetWrappingR() const { return m_R; }

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);
		void SetWrappingR(Wrapping wrapping);

		Filter GetMagFilter() const { return m_Mag; }
		Filter GetMinFilter() const { return m_Min; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter min);
		void SetMagFilter(Filter mag);

		Color GetBorderColor() const { return m_BorderColor; }
		void SetBorderColor(const Color &color);

		void Bind() const;

		void GenerateMipMaps() const;
		bool IsMipMapsGenerated() const { return m_MipmapGenerated; }

		void Create(uint32_t width, uint32_t height);
		void Create(const Image &image);

		Vector2u Size() const { return m_Size; }
		uint32_t Width() const { return m_Size.Width; }
		uint32_t Height() const { return m_Size.Height; }

		InternalFormat GetInternalFormat() const { return m_InternalFormat; }

		Image ToImage() const;

		void Update(const float *pixels) { Update(pixels, m_Size.Width, m_Size.Height, 0, 0); }
		void Update(const float *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y);

		void Update(const glm::vec4 *pixels) { Update(pixels, m_Size.Width, m_Size.Height, 0, 0); }
		void Update(const glm::vec4 *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y);

		void Update(const Texture &texture) { Update(texture, 0, 0); }
		void Update(const Texture &texture, const uint32_t x, const uint32_t y) { Update(texture.ToImage(), x, y); };

		void Update(const Image &image) { Update(image.GetPixels(), image.Width(), image.Height(), 0, 0); };

		void Update(const Image &image, const uint32_t x, const uint32_t y)
		{
			Update(image.GetPixels(), image.Width(), image.Height(), x, y);
		};

		void Swap(Texture &right) noexcept;

		static uint32_t GetMaximumSize() { return GetMaxSize(); }
		static uint32_t GetMaxSize();

		bool operator==(const Texture &texture) const
		{
			return *m_Texture == *texture.m_Texture;
		}
	};
}
