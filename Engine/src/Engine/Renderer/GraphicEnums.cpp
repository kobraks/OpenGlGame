#include "pch.h"
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	const char* ToString(WrapMode mode) {
		switch (mode) {
		case WrapMode::Repeat: return "Repeat";
		case WrapMode::ClampEdge: return "ClampEdge";
		case WrapMode::ClampBorder: return "ClampBorder";
		case WrapMode::MirroredRepeat: return "MirroredRepeat";
		default: return "Unknown";
		}
	}

	const char* ToString(FilterMode mode) {
		switch (mode) {
		case FilterMode::Nearest: return "Nearest";
		case FilterMode::Linear: return "Linear";
		case FilterMode::NearestMipmapNearest: return "NearestMipmapNearest";
		case FilterMode::LinearMipmapNearest: return "LinearMipmapNearest";
		case FilterMode::NearestMipmapLinear: return "NearestMipmapLinear";
		case FilterMode::LinearMipmapLinear: return "LinearMipmapLinear";
		default: return "Unknown";
		}
	}

	const char* ToString(BlitFilter filter) {
		switch (filter) {
		case BlitFilter::Nearest: return "Nearest";
		case BlitFilter::Linear: return "Linear";
		default: return "Unknown";
		}
	}

	const char* ToString(BlitMask mask) {
		switch (mask) {
		case BlitMask::Color: return "Color";
		case BlitMask::Depth: return "Depth";
		case BlitMask::ColorAndDepth: return "ColorAndDepth";
		default: return "Unknown";
		}
	}

	const char* ToString(DataFormat format) {
		switch (format) {
		case DataFormat::Red: return "Red";
		case DataFormat::RG: return "RG";
		case DataFormat::RGB: return "RGB";
		case DataFormat::BGR: return "BGR";
		case DataFormat::RGBA: return "RGBA";
		case DataFormat::BGRA: return "BGRA";
		case DataFormat::RedInteger: return "RedInteger";
		case DataFormat::RGInteger: return "RGInteger";
		case DataFormat::RGBInteger: return "RGBInteger";
		case DataFormat::BGRInteger: return "BGRInteger";
		case DataFormat::RGBAInteger: return "RGBAInteger";
		case DataFormat::BGRAInteger: return "BGRAInteger";
		case DataFormat::StencilIndex: return "StencilIndex";
		case DataFormat::DepthComponent: return "DepthComponent";
		case DataFormat::DepthStencil: return "DepthStencil";
		default: return "Unknown";
		}
	}

	const char* ToString(ImageFormat format) {
		switch (format) {
#define CASE(val) case ImageFormat::val: return #val
		CASE(CompressedRed);
		CASE(CompressedRedRGTC1);
		CASE(CompressedRG);
		CASE(CompressedRGB);
		CASE(CompressedRGBA);
		CASE(CompressedRGRGTC2);
		CASE(CompressedSignedRedRGTC1);
		CASE(CompressedSignedRGRGTC2);
		CASE(CompressedSRGB);
		CASE(DepthComponent);
		CASE(DepthComponent16);
		CASE(DepthComponent24);
		CASE(DepthComponent32);
		CASE(DepthComponent32F);
		CASE(DepthStencil);
		CASE(Depth24Stencil8);
		CASE(Depth32FStencil8);
		CASE(StencilIndex);
		CASE(StencilIndex1);
		CASE(StencilIndex4);
		CASE(StencilIndex8);
		CASE(StencilIndex16);
		CASE(R16SNorm);
		CASE(R16F);
		CASE(R16I);
		CASE(R16UI);
		CASE(R32F);
		CASE(R32I);
		CASE(R32UI);
		CASE(R3G3B2);
		CASE(R8);
		CASE(R8I);
		CASE(R8SNorm);
		CASE(R8UI);
		CASE(Red);
		CASE(RG);
		CASE(RG16);
		CASE(RG16F);
		CASE(RG16SNorm);
		CASE(RG32F);
		CASE(RG32I);
		CASE(RG32UI);
		CASE(RG8);
		CASE(RG8I);
		CASE(RG8SNorm);
		CASE(RG8UI);
		CASE(RGB);
		CASE(RGB10);
		CASE(RGB10A2);
		CASE(RGB12);
		CASE(RGB16);
		CASE(RGB16F);
		CASE(RGB16I);
		CASE(RGB16UI);
		CASE(RGB32F);
		CASE(RGB32I);
		CASE(RGB32UI);
		CASE(RGB4);
		CASE(RGB5);
		CASE(RGB5A1);
		CASE(RGB8);
		CASE(RGB8I);
		CASE(RGB8UI);
		CASE(RGB9E5);
		CASE(RGBA);
		CASE(RGBA12);
		CASE(RGBA16);
		CASE(RGBA16F);
		CASE(RGBA16I);
		CASE(RGBA16UI);
		CASE(RGBA2);
		CASE(RGBA32F);
		CASE(RGBA32I);
		CASE(RGBA32UI);
		CASE(RGBA4);
		CASE(RGBA8);
		CASE(RGBA8I);
		CASE(RGBA8UI);
		CASE(SRGB8);
		CASE(SRGB8A8);
		CASE(SRGBA);
#undef CASE
		default: return "Unknown";
		}
	}

	const char* ToString(DataType type) {
		switch (type) {
		case DataType::Byte: return "Byte";
		case DataType::UnsignedByte: return "UnsignedByte";
		case DataType::Short: return "Short";
		case DataType::UnsignedShort: return "UnsignedShort";
		case DataType::Int: return "Int";
		case DataType::UnsignedInt: return "UnsignedInt";
		case DataType::Float: return "Float";
		case DataType::Double: return "Double";
		default: return "Unknown";
		}
	}

	const char* ToString(TextureUsage usage) {
		switch (usage) {
		case TextureUsage::Default: return "Default";
		case TextureUsage::RenderTarget: return "RenderTarget";
		case TextureUsage::DepthStencil: return "DepthStencil";
		case TextureUsage::Storage: return "Storage";
		default: return "Unknown";
		}
	}

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

	TextureUsage GetUsageFromFormat(ImageFormat format) {
		if (IsDepthFormat(format)) return TextureUsage::DepthStencil;
		if (IsStorageCapable(format)) return TextureUsage::Storage;
		return TextureUsage::RenderTarget;
	}
}
