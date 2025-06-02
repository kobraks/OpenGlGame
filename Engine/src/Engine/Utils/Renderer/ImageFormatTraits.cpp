#include "pch.h"
#include "ImageFormatTraits.h"

namespace Engine::Utils {
	bool IsDepthFormat(ImageFormat format) {
		switch (format) {
		case ImageFormat::DepthComponent:
		case ImageFormat::DepthComponent16:
		case ImageFormat::DepthComponent24:
		case ImageFormat::DepthComponent32:
		case ImageFormat::DepthComponent32F:
		case ImageFormat::DepthStencil:
		case ImageFormat::Depth24Stencil8:
		case ImageFormat::Depth32FStencil8:
		case ImageFormat::StencilIndex:
		case ImageFormat::StencilIndex1:
		case ImageFormat::StencilIndex4:
		case ImageFormat::StencilIndex8:
		case ImageFormat::StencilIndex16:
			return true;
		default:
			return false;
		}
	}

	bool IsStorageCapable(ImageFormat format) {
		switch (format) {
		case ImageFormat::R32UI:
		case ImageFormat::RGBA32F:
		case ImageFormat::RGBA32I:
		case ImageFormat::RGBA32UI:
		case ImageFormat::R32I:
		case ImageFormat::R32F:
			return true;
		default:
			return false;
		}
	}

	bool IsStencilOnlyFormat(ImageFormat format) {
		switch (format) {
		case ImageFormat::StencilIndex:
		case ImageFormat::StencilIndex1:
		case ImageFormat::StencilIndex4:
		case ImageFormat::StencilIndex8:
		case ImageFormat::StencilIndex16:
			return true;
		default:
			return false;
		}
	}

	bool IsDepthStencilFormat(ImageFormat format) {
		switch (format) {
		case ImageFormat::DepthStencil:
		case ImageFormat::Depth24Stencil8:
		case ImageFormat::Depth32FStencil8:
			return true;
		default:
			return false;
		}
	}

	bool IsColorFormat(ImageFormat format) {
		return !IsDepthFormat(format);
	}

	TextureUsage FormatUsageMapping(ImageFormat format) {
		if (IsDepthFormat(format)) return TextureUsage::DepthStencil;
		if (IsStorageCapable(format)) return TextureUsage::Storage;
		return TextureUsage::RenderTarget;
	}

	std::pair<DataFormat, DataType> GetDefaultFormatAndType(ImageFormat format) {
		switch (format) {
		case ImageFormat::Red:
		case ImageFormat::R8:
			return { DataFormat::Red, DataType::UnsignedByte };

		case ImageFormat::RG:
		case ImageFormat::RG8:
			return { DataFormat::RG, DataType::UnsignedByte };

		case ImageFormat::RGB:
		case ImageFormat::RGB8:
			return { DataFormat::RGB, DataType::UnsignedByte };

		case ImageFormat::RGBA:
		case ImageFormat::RGBA8:
			return { DataFormat::RGBA, DataType::UnsignedByte };

		case ImageFormat::SRGB8:
			return { DataFormat::RGB, DataType::UnsignedByte };

		case ImageFormat::SRGB8A8:
			return { DataFormat::RGBA, DataType::UnsignedByte };

			// Float formats
		case ImageFormat::R16F:
		case ImageFormat::R32F:
			return { DataFormat::Red, DataType::Float };

		case ImageFormat::RG16F:
		case ImageFormat::RG32F:
			return { DataFormat::RG, DataType::Float };

		case ImageFormat::RGB16F:
		case ImageFormat::RGB32F:
			return { DataFormat::RGB, DataType::Float };

		case ImageFormat::RGBA16F:
		case ImageFormat::RGBA32F:
			return { DataFormat::RGBA, DataType::Float };

			// Integer formats
		case ImageFormat::R8I:
		case ImageFormat::R32I:
			return { DataFormat::RedInteger, DataType::Int };

		case ImageFormat::RG8I:
		case ImageFormat::RG32I:
			return { DataFormat::RGInteger, DataType::Int };

		case ImageFormat::RGB8I:
		case ImageFormat::RGB32I:
			return { DataFormat::RGBInteger, DataType::Int };

		case ImageFormat::RGBA8I:
		case ImageFormat::RGBA32I:
			return { DataFormat::RGBAInteger, DataType::Int };

			// Depth formats
		case ImageFormat::DepthComponent:
		case ImageFormat::DepthComponent16:
		case ImageFormat::DepthComponent24:
		case ImageFormat::DepthComponent32:
		case ImageFormat::DepthComponent32F:
			return { DataFormat::DepthComponent, DataType::Float };

			// Depth-stencil
		case ImageFormat::Depth24Stencil8:
		case ImageFormat::Depth32FStencil8:
			return { DataFormat::DepthStencil, DataType::UnsignedInt };

		default:
			LOG_ENGINE_WARN("No default format/type mapping for ImageFormat {}", static_cast<uint32_t>(format));
			return { DataFormat::RGBA, DataType::UnsignedByte }; // safe fallback
		}
	}
}