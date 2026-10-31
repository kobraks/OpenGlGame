#include "pch.h"
#include "ImageFormatTraits.h"

namespace Engine::Utils {
	bool IsDepthFormat(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::DepthComponent:
		case TextureInternalFormat::DepthComponent16:
		case TextureInternalFormat::DepthComponent24:
		case TextureInternalFormat::DepthComponent32:
		case TextureInternalFormat::DepthComponent32F:
		case TextureInternalFormat::DepthStencil:
		case TextureInternalFormat::Depth24Stencil8:
		case TextureInternalFormat::Depth32FStencil8:
		case TextureInternalFormat::StencilIndex:
		case TextureInternalFormat::StencilIndex1:
		case TextureInternalFormat::StencilIndex4:
		case TextureInternalFormat::StencilIndex8:
		case TextureInternalFormat::StencilIndex16:
			return true;
		default:
			return false;
		}
	}

	bool IsStorageCapable(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::R32UI:
		case TextureInternalFormat::RGBA32F:
		case TextureInternalFormat::RGBA32I:
		case TextureInternalFormat::RGBA32UI:
		case TextureInternalFormat::R32I:
		case TextureInternalFormat::R32F:
			return true;
		default:
			return false;
		}
	}

	bool IsStencilOnlyFormat(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::StencilIndex:
		case TextureInternalFormat::StencilIndex1:
		case TextureInternalFormat::StencilIndex4:
		case TextureInternalFormat::StencilIndex8:
		case TextureInternalFormat::StencilIndex16:
			return true;
		default:
			return false;
		}
	}

	bool IsDepthStencilFormat(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::DepthStencil:
		case TextureInternalFormat::Depth24Stencil8:
		case TextureInternalFormat::Depth32FStencil8:
			return true;
		default:
			return false;
		}
	}

	bool IsDepthOnlyFormat(TextureInternalFormat format) {
		return IsDepthFormat(format) && !IsDepthStencilFormat(format) && !IsStencilOnlyFormat(format);
	}

	bool HasDepthAspect(TextureInternalFormat format) {
		return IsDepthOnlyFormat(format) || IsDepthStencilFormat(format);
	}

	bool HasStencilAspect(TextureInternalFormat format) {
		return IsStencilOnlyFormat(format) || IsDepthStencilFormat(format);
	}

	bool IsIntegerColorFormat(TextureInternalFormat format) {
		switch (format) {
			// Signed
			// 8-bit
		case TextureInternalFormat::R8I:
		case TextureInternalFormat::RG8I:
		case TextureInternalFormat::RGB8I:
		case TextureInternalFormat::RGBA8I:
			// 16-bit
		case TextureInternalFormat::R16I:
		case TextureInternalFormat::RG16I:
		case TextureInternalFormat::RGB16I:
		case TextureInternalFormat::RGBA16I:
			// 32-bit
		case TextureInternalFormat::R32I:
		case TextureInternalFormat::RG32I:
		case TextureInternalFormat::RGB32I:
		case TextureInternalFormat::RGBA32I:
			// Unsigned
			// 8-bit
		case TextureInternalFormat::R8UI:
		case TextureInternalFormat::RG8UI:
		case TextureInternalFormat::RGB8UI:
		case TextureInternalFormat::RGBA8UI:
			// 16-bit
		case TextureInternalFormat::R16UI:
		case TextureInternalFormat::RG16UI:
		case TextureInternalFormat::RGB16UI:
		case TextureInternalFormat::RGBA16UI:
			// 32-bit
		case TextureInternalFormat::R32UI:
		case TextureInternalFormat::RG32UI:
		case TextureInternalFormat::RGB32UI:
		case TextureInternalFormat::RGBA32UI:
			return true;

		default:
			return false;
		}
	}

	bool IsSRGBFormat(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::SRGB8:
		case TextureInternalFormat::SRGB8A8:
		case TextureInternalFormat::SRGBA:
			return true;
		default:
			return false;
		}
	}

	bool IsColorFormat(TextureInternalFormat format) {
		return !IsDepthFormat(format);
	}

	TextureUsage FormatUsageMapping(TextureInternalFormat format) {
		if (IsDepthFormat(format)) return TextureUsage::DepthStencil;
		if (IsStorageCapable(format)) return TextureUsage::Storage;
		return TextureUsage::RenderTarget;
	}

	std::pair<PixelFormat, PixelDataType> GetDefaultFormatAndType(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::Red:
		case TextureInternalFormat::R8:
			return { PixelFormat::Red, PixelDataType::UnsignedByte };

		case TextureInternalFormat::RG:
		case TextureInternalFormat::RG8:
			return { PixelFormat::RG, PixelDataType::UnsignedByte };

		case TextureInternalFormat::RGB:
		case TextureInternalFormat::RGB8:
			return { PixelFormat::RGB, PixelDataType::UnsignedByte };

		case TextureInternalFormat::RGBA:
		case TextureInternalFormat::RGBA8:
			return { PixelFormat::RGBA, PixelDataType::UnsignedByte };

		case TextureInternalFormat::SRGB8:
			return { PixelFormat::RGB, PixelDataType::UnsignedByte };

		case TextureInternalFormat::SRGB8A8:
			return { PixelFormat::RGBA, PixelDataType::UnsignedByte };

			// Float formats
		case TextureInternalFormat::R16F:
		case TextureInternalFormat::R32F:
			return { PixelFormat::Red, PixelDataType::Float };

		case TextureInternalFormat::RG16F:
		case TextureInternalFormat::RG32F:
			return { PixelFormat::RG, PixelDataType::Float };

		case TextureInternalFormat::RGB16F:
		case TextureInternalFormat::RGB32F:
			return { PixelFormat::RGB, PixelDataType::Float };

		case TextureInternalFormat::RGBA16F:
		case TextureInternalFormat::RGBA32F:
			return { PixelFormat::RGBA, PixelDataType::Float };

			// Integer formats
		case TextureInternalFormat::R8I:
		case TextureInternalFormat::R16I:
		case TextureInternalFormat::R32I:
			return { PixelFormat::RedInteger, PixelDataType::Int };

		case TextureInternalFormat::RG8I:
		case TextureInternalFormat::RG16I:
		case TextureInternalFormat::RG32I:
			return { PixelFormat::RGInteger, PixelDataType::Int };

		case TextureInternalFormat::RGB8I:
		case TextureInternalFormat::RGB16I:
		case TextureInternalFormat::RGB32I:
			return { PixelFormat::RGBInteger, PixelDataType::Int };

		case TextureInternalFormat::RGBA8I:
		case TextureInternalFormat::RGBA16I:
		case TextureInternalFormat::RGBA32I:
			return { PixelFormat::RGBAInteger, PixelDataType::Int };

			// Unsigned integer formats
		case TextureInternalFormat::R8UI:
		case TextureInternalFormat::R16UI:
		case TextureInternalFormat::R32UI:
			return { PixelFormat::RedInteger, PixelDataType::UnsignedInt };

		case TextureInternalFormat::RG8UI:
		case TextureInternalFormat::RG16UI:
		case TextureInternalFormat::RG32UI:
			return { PixelFormat::RGInteger, PixelDataType::UnsignedInt };

		case TextureInternalFormat::RGB8UI:
		case TextureInternalFormat::RGB16UI:
		case TextureInternalFormat::RGB32UI:
			return { PixelFormat::RGBInteger, PixelDataType::UnsignedInt };

		case TextureInternalFormat::RGBA8UI:
		case TextureInternalFormat::RGBA16UI:
		case TextureInternalFormat::RGBA32UI:
			return { PixelFormat::RGBAInteger, PixelDataType::UnsignedInt };

			// Depth formats
		case TextureInternalFormat::DepthComponent:
		case TextureInternalFormat::DepthComponent16:
		case TextureInternalFormat::DepthComponent24:
		case TextureInternalFormat::DepthComponent32:
		case TextureInternalFormat::DepthComponent32F:
			return { PixelFormat::DepthComponent, PixelDataType::Float };

			// Depth-stencil
		case TextureInternalFormat::Depth24Stencil8:
		case TextureInternalFormat::Depth32FStencil8:
			return { PixelFormat::DepthStencil, PixelDataType::UnsignedInt };

		default:
			LOG_ENGINE_WARN("No default format/type mapping for TextureInternalFormat {}", static_cast<uint32_t>(format));
			return { PixelFormat::RGBA, PixelDataType::UnsignedByte }; // safe fallback
		}
	}
}