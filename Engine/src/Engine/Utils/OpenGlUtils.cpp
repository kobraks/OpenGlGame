#include "pch.h"
#include "OpenGlUtils.h"

#include <glad/glad.h>

namespace Engine::Utils {
	uint32_t ToGLImageFormat(ImageFormat format) {
		switch (format) {
		case ImageFormat::CompressedRed:
			return GL_COMPRESSED_RED;
		case ImageFormat::CompressedRedRGTC1:
			return GL_COMPRESSED_RED_RGTC1;
		case ImageFormat::CompressedRG:
			return GL_COMPRESSED_RG;
		case ImageFormat::CompressedRGB:
			return GL_COMPRESSED_RGB;
		case ImageFormat::CompressedRGBA:
			return GL_COMPRESSED_RGBA;
		case ImageFormat::CompressedRGRGTC2:
			return GL_COMPRESSED_RG_RGTC2;
		case ImageFormat::CompressedSignedRedRGTC1:
			return GL_COMPRESSED_SIGNED_RED_RGTC1;
		case ImageFormat::CompressedSignedRGRGTC2:
			return GL_COMPRESSED_SIGNED_RG_RGTC2;
		case ImageFormat::CompressedSRGB:
			return GL_COMPRESSED_SRGB;
		case ImageFormat::DepthStencil:
			return GL_DEPTH_STENCIL;
		case ImageFormat::Depth24Stencil8:
			return GL_DEPTH24_STENCIL8;
		case ImageFormat::Depth32FStencil8:
			return GL_DEPTH32F_STENCIL8;
		case ImageFormat::DepthComponent:
			return GL_DEPTH_COMPONENT;
		case ImageFormat::DepthComponent16:
			return GL_DEPTH_COMPONENT16;
		case ImageFormat::DepthComponent24:
			return GL_DEPTH_COMPONENT24;
		case ImageFormat::DepthComponent32F:
			return GL_DEPTH_COMPONENT32F;
		case ImageFormat::DepthComponent32:
			return GL_DEPTH_COMPONENT32;
		case ImageFormat::StencilIndex:
			return GL_STENCIL_INDEX;
		case ImageFormat::StencilIndex1:
			return GL_STENCIL_INDEX1;
		case ImageFormat::StencilIndex4:
			return GL_STENCIL_INDEX4;
		case ImageFormat::StencilIndex8:
			return GL_STENCIL_INDEX8;
		case ImageFormat::StencilIndex16:
			return GL_STENCIL_INDEX8;
		case ImageFormat::R16F:
			return GL_R16F;
		case ImageFormat::R16I:
			return GL_R16I;
		case ImageFormat::R16SNorm:
			return GL_R16_SNORM;
		case ImageFormat::R16UI:
			return GL_R16UI;
		case ImageFormat::R32F:
			return GL_R32F;
		case ImageFormat::R32I:
			return GL_R32I;
		case ImageFormat::R32UI:
			return GL_R32UI;
		case ImageFormat::R3G3B2:
			return GL_R3_G3_B2;
		case ImageFormat::R8:
			return GL_R8;
		case ImageFormat::R8I:
			return GL_R8I;
		case ImageFormat::R8SNorm:
			return GL_R8_SNORM;
		case ImageFormat::R8UI:
			return GL_R8UI;
		case ImageFormat::Red:
			return GL_RED;
		case ImageFormat::RG:
			return GL_RG;
		case ImageFormat::RG16:
			return GL_RG16;
		case ImageFormat::RG16F:
			return GL_RG16F;
		case ImageFormat::RG16SNorm:
			return GL_RG16_SNORM;
		case ImageFormat::RG32F:
			return GL_RG32F;
		case ImageFormat::RG32I:
			return GL_RG32I;
		case ImageFormat::RG32UI:
			return GL_RG32UI;
		case ImageFormat::RG8:
			return GL_RG8;
		case ImageFormat::RG8I:
			return GL_RGB8I;
		case ImageFormat::RG8SNorm:
			return GL_RG8_SNORM;
		case ImageFormat::RG8UI:
			return GL_RG8UI;
		case ImageFormat::RGB:
			return GL_RGB;
		case ImageFormat::RGB10:
			return GL_RGB10;
		case ImageFormat::RGB10A2:
			return GL_RGB10_A2;
		case ImageFormat::RGB12:
			return GL_RGB12;
		case ImageFormat::RGB16:
			return GL_RGB16;
		case ImageFormat::RGB16F:
			return GL_RGB16F;
		case ImageFormat::RGB16I:
			return GL_RGB16I;
		case ImageFormat::RGB16UI:
			return GL_RGB16UI;
		case ImageFormat::RGB32F:
			return GL_RGB32F;
		case ImageFormat::RGB32I:
			return GL_RGB32I;
		case ImageFormat::RGB32UI:
			return GL_RGB32UI;
		case ImageFormat::RGB4:
			return GL_RGB4;
		case ImageFormat::RGB5:
			return GL_RGB5;
		case ImageFormat::RGB5A1:
			return GL_RGB5_A1;
		case ImageFormat::RGB8:
			return GL_RGB8;
		case ImageFormat::RGB8I:
			return GL_RGB8I;
		case ImageFormat::RGB8UI:
			return GL_RGB8UI;
		case ImageFormat::RGB9E5:
			return GL_RGB9_E5;
		case ImageFormat::RGBA:
			return GL_RGBA;
		case ImageFormat::RGBA12:
			return GL_RGBA12;
		case ImageFormat::RGBA16:
			return GL_RGBA16;
		case ImageFormat::RGBA16F:
			return GL_RGBA16F;
		case ImageFormat::RGBA16I:
			return GL_RGBA16I;
		case ImageFormat::RGBA16UI:
			return GL_RGBA16UI;
		case ImageFormat::RGBA2:
			return GL_RGBA2;
		case ImageFormat::RGBA32F:
			return GL_RGBA32F;
		case ImageFormat::RGBA32I:
			return GL_RGBA32I;
		case ImageFormat::RGBA32UI:
			return GL_RGBA32UI;
		case ImageFormat::RGBA4:
			return GL_RGBA4;
		case ImageFormat::RGBA8:
			return GL_RGBA8;
		case ImageFormat::RGBA8I:
			return GL_RGBA8I;
		case ImageFormat::RGBA8UI:
			return GL_RGBA8I;
		case ImageFormat::SRGB8:
			return GL_SRGB8;
		case ImageFormat::SRGB8A8:
			return GL_SRGB8_ALPHA8;
		case ImageFormat::SRGBA:
			return GL_SRGB_ALPHA;
		}
	}

	uint32_t ToGLFilterMode(FilterMode filter) {
		switch (filter) {
		case FilterMode::Nearest:
			return GL_NEAREST;
		case FilterMode::Linear:
			return GL_LINEAR;
		case FilterMode::NearestMipmapNearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode::LinearMipmapNearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode::NearestMipmapLinear:
			return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode::LinearMipmapLinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		}
	}

	uint32_t ToGLWrapMode(WrapMode wrapMode) {
		switch (wrapMode) {
		case WrapMode::Repeat:
			return GL_REPEAT;
		case WrapMode::MirroredRepeat:
			return GL_MIRRORED_REPEAT;
		case WrapMode::ClampEdge:
			return GL_CLAMP_TO_EDGE;
		case WrapMode::ClampBorder:
			return GL_CLAMP_TO_BORDER;
		}
	}

	uint32_t ToGLDataFormat(DataFormat format) {
		switch (format) {
		case DataFormat::Red:
			return GL_RED;
		case DataFormat::RG:
			return GL_RG;
		case DataFormat::RGB:
			return GL_RGB;
		case DataFormat::BGR:
			return GL_BGR;
		case DataFormat::RGBA:
			return GL_RGBA;
		case DataFormat::BGRA:
			return GL_BGRA;
		case DataFormat::RedInteger:
			return GL_RED_INTEGER;
		case DataFormat::RGInteger:
			return GL_RG_INTEGER;
		case DataFormat::RGBInteger:
			return GL_RGB_INTEGER;
		case DataFormat::BGRInteger:
			return GL_BGR_INTEGER;
		case DataFormat::RGBAInteger:
			return GL_RGBA_INTEGER;
		case DataFormat::BGRAInteger:
			return GL_BGRA_INTEGER;
		case DataFormat::StencilIndex:
			return GL_STENCIL_INDEX;
		case DataFormat::DepthComponent:
			return GL_DEPTH_COMPONENT;
		case DataFormat::DepthStencil:
			return GL_DEPTH_STENCIL;
		}
	}

	uint32_t ToGLDataType(DataType type) {
		switch (type) {
		case DataType::Byte:
			return GL_BYTE;
		case DataType::UnsignedByte:
			return GL_UNSIGNED_BYTE;
		case DataType::Short:
			return GL_SHORT;
		case DataType::UnsignedShort:
			return GL_UNSIGNED_SHORT;
		case DataType::Int:
			return GL_INT;
		case DataType::UnsignedInt:
			return GL_UNSIGNED_INT;
		case DataType::Float:
			return GL_FLOAT;
		case DataType::Double:
			return GL_DOUBLE;
		}
	}

	uint32_t ToGLFilter(BlitFilter filter) {
		switch (filter) {
		case BlitFilter::Nearest:
			return GL_NEAREST;
		case BlitFilter::Linear:
			return GL_LINEAR;
		}
	}

	uint32_t ToGLMask(BlitMask mask) {
		switch (mask) {
		case BlitMask::Color:
			return GL_COLOR_BUFFER_BIT;
		case BlitMask::Depth:
			return GL_DEPTH_BUFFER_BIT;
		case BlitMask::ColorAndDepth:
			return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		}
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
