#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	enum class WrapMode {
		Repeat = 0,
		ClampEdge,
		ClampBorder,
		MirroredRepeat,
	};

	enum class FilterMode {
		Nearest = 0,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	enum class BlitFilter {
		Nearest = 0,
		Linear
	};

	enum class BlitMask {
		Color = 0x1,
		Depth = 0x2,
		ColorAndDepth = 0x3
	};

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

	enum class ImageFormat : uint32_t {
		CompressedRed,
		CompressedRedRGTC1,
		CompressedRG,
		CompressedRGB,
		CompressedRGBA,
		CompressedRGRGTC2,
		CompressedSignedRedRGTC1,
		CompressedSignedRGRGTC2,
		CompressedSRGB,

		DepthComponent,
		DepthComponent16,
		DepthComponent24,
		DepthComponent32,
		DepthComponent32F,

		DepthStencil,
		Depth24Stencil8,
		Depth32FStencil8,

		StencilIndex,
		StencilIndex1,
		StencilIndex4,
		StencilIndex8,
		StencilIndex16,

		R16SNorm,
		R16F,
		R16I,
		R16UI,
		R32F,
		R32I,
		R32UI,
		R3G3B2,

		R8,
		R8I,
		R8SNorm,
		R8UI,
		Red,

		RG,
		RG16,
		RG16F,
		RG16SNorm,
		RG32F,
		RG32I,
		RG32UI,

		RG8,
		RG8I,
		RG8SNorm,
		RG8UI,

		RGB,
		RGB10,
		RGB10A2,
		RGB12,
		RGB16,
		RGB16F,
		RGB16I,
		RGB16UI,

		RGB32F,
		RGB32I,
		RGB32UI,
		RGB4,
		RGB5,
		RGB5A1,
		RGB8,
		RGB8I,
		RGB8UI,
		RGB9E5,
		RGBA,
		RGBA12,
		RGBA16,
		RGBA16F,
		RGBA16I,
		RGBA16UI,
		RGBA2,
		RGBA32F,
		RGBA32I,
		RGBA32UI,
		RGBA4,
		RGBA8,
		RGBA8I,
		RGBA8UI,
		SRGB8,
		SRGB8A8,
		SRGBA
	};

	enum class DataType : uint32_t {
		Byte, UnsignedByte,
		Short, UnsignedShort,
		Int, UnsignedInt,
		Float, Double
	};

	enum class TextureUsage {
		Default,
		RenderTarget,
		DepthStencil,
		Storage
	};

	enum class CompareFunction : uint8_t{
		Never = 0,
		Less,
		LEqual,
		Greater,
		GEqual,
		Equal,
		NotEqual,
		Always
	}; 

	enum class UniformKind : uint8_t { Unknown = 0, Scalar, Vector, Matrix, Sampler, Image, AtomicCounter };
	enum class ScalarKind : uint8_t { None = 0, Bool, Int, UInt, Float, Double };
	enum class TextureDim : uint8_t { None = 0, D1, D2, D3, Cube, Rect, Buffer, D1Array, D2Array, CubeArray, D2MS, D2MSArray };

	struct UniformTypeDesc {
		UniformKind Kind = UniformKind::Unknown;
		ScalarKind Scalar = ScalarKind::None;

		uint8_t Rows = 0; // vectors: length; matrices: Rows; scalars: 1
		uint8_t Cols = 0; // matrices: Cols; vectors/scalars: 1

		TextureDim Dim = TextureDim::None; //Only for samplers and images
		bool Shadow = false; //Only for samplers
	};
}