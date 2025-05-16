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

	namespace Utils {
		const char* ToString(WrapMode mode);
		const char* ToString(FilterMode mode);
		const char* ToString(BlitFilter filter);
		const char* ToString(BlitMask mask);
		const char* ToString(DataFormat format);
		const char* ToString(ImageFormat format);
		const char* ToString(DataType type);
		const char* ToString(TextureUsage usage);

		bool IsDepthFormat(ImageFormat format);
		bool IsStorageCapable(ImageFormat format);

		bool IsStencilOnlyFormat(ImageFormat format);
		bool IsDepthStencilFormat(ImageFormat format);

		bool IsColorFormat(ImageFormat format);

		TextureUsage GetUsageFromFormat(ImageFormat format);
	}
}

template <>
struct fmt::formatter<Engine::WrapMode> : fmt::formatter<const char*> {
	auto format(Engine::WrapMode v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::FilterMode> : fmt::formatter<const char*> {
	auto format(Engine::FilterMode v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitFilter> : fmt::formatter<const char*> {
	auto format(Engine::BlitFilter v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitMask> : fmt::formatter<const char*> {
	auto format(Engine::BlitMask v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataFormat> : fmt::formatter<const char*> {
	auto format(Engine::DataFormat v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::ImageFormat> : fmt::formatter<const char*> {
	auto format(Engine::ImageFormat v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataType> : fmt::formatter<const char*> {
	auto format(Engine::DataType v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::TextureUsage> : fmt::formatter<const char*> {
	auto format(Engine::TextureUsage v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};