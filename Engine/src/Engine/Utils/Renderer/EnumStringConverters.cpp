#include "pch.h"
#include "EnumStringConverters.h"

namespace Engine::Utils {
	std::string_view ToString(WrapMode mode) {
		switch (mode) {
		case WrapMode::Repeat: return "Repeat";
		case WrapMode::ClampEdge: return "ClampEdge";
		case WrapMode::ClampBorder: return "ClampBorder";
		case WrapMode::MirroredRepeat: return "MirroredRepeat";
		default: return "Unknown";
		}
	}

	std::string_view ToString(FilterMode mode) {
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

	std::string_view ToString(BlitFilter filter) {
		switch (filter) {
		case BlitFilter::Nearest: return "Nearest";
		case BlitFilter::Linear: return "Linear";
		default: return "Unknown";
		}
	}

	std::string_view ToString(BlitMask mask) {
		switch (mask) {
		case BlitMask::Color: return "Color";
		case BlitMask::Depth: return "Depth";
		case BlitMask::ColorAndDepth: return "ColorAndDepth";
		default: return "Unknown";
		}
	}

	std::string_view ToString(DataFormat format) {
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

	std::string_view ToString(ImageFormat format) {
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
			CASE(RG16I);
			CASE(RG16UI);
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

	std::string_view ToString(DataType type) {
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

	std::string_view ToString(TextureUsage usage) {
		switch (usage) {
		case TextureUsage::Default: return "Default";
		case TextureUsage::RenderTarget: return "RenderTarget";
		case TextureUsage::DepthStencil: return "DepthStencil";
		case TextureUsage::Storage: return "Storage";
		default: return "Unknown";
		}
	}

	std::string_view ToString(BufferUsage usage) {
		switch (usage) {
		case BufferUsage::StaticDraw: return "StaticDraw";
		case BufferUsage::StaticRead: return "StaticRead";
		case BufferUsage::StaticCopy: return "StaticCopy";

		case BufferUsage::DynamicDraw: return "DynamicDraw";
		case BufferUsage::DynamicRead: return "DynamicRead";
		case BufferUsage::DynamicCopy: return "DynamicCopy";

		case BufferUsage::StreamDraw: return "StreamDraw";
		case BufferUsage::StreamRead: return "StreamRead";
		case BufferUsage::StreamCopy: return "StreamCopy";
		}
	}

	std::string ToString(BufferStorageFlags flags) {
		if (flags == BufferStorageFlags::None)
			return "None";

		std::string result;
		result.reserve(46);

		auto append = [&](std::string_view name) {
			if (!result.empty()) result += " | ";
			result.append(name.data(), name.size());
			};

		if (HasFlag(flags, BufferStorageFlags::Dynamic)) append("Dynamic");
		if (HasFlag(flags, BufferStorageFlags::MapRead)) append("MapRead");
		if (HasFlag(flags, BufferStorageFlags::MapWrite)) append("MapWrite");
		if (HasFlag(flags, BufferStorageFlags::MapPersistent)) append("MapPersistent");
		if (HasFlag(flags, BufferStorageFlags::MapCoherent)) append("MapCoherent");
		if (HasFlag(flags, BufferStorageFlags::ClientStorage)) append("ClientStorage");

		return result;
	}

	std::string_view ToString(BufferStorageMode mode) {
		switch (mode) {
		case BufferStorageMode::Immutable:
			return "Immutable";
		case BufferStorageMode::Mutable:
			return "Mutable";
		}
		return "Unknown";
	}

	std::string_view ToString(BufferAccess access) {
		switch (access) {
		case BufferAccess::ReadOnly:
			return "ReadOnly";
		case BufferAccess::WriteOnly:
			return "WriteOnly";
		case BufferAccess::ReadWrite:
			return "ReadWrite";
		}
		return "Unknown";
	}

	std::string_view ToString(BufferTarget target) {
		switch (target) {
		case BufferTarget::Array:
			return "ArrayBuffer";
		case BufferTarget::ElementArray:
			return "ElementArrayBuffer";
		case BufferTarget::Uniform:
			return "UniformBuffer";
		case BufferTarget::ShaderStorage:
			return "ShaderStorageBuffer";
		case BufferTarget::CopyRead:
			return "CopyReadBuffer";
		case BufferTarget::CopyWrite:
			return "CopyWriteBuffer";
		case BufferTarget::PixelPack:
			return "PixelPackBuffer";
		case BufferTarget::PixelUnpack:
			return "PixelUnpackBuffer";
		case BufferTarget::DrawIndirect:
			return "DrawIndirectBuffer";
		case BufferTarget::DispatchIndirect:
			return "DispatchIndirectBuffer";
		case BufferTarget::TransformFeedback:
			return "TransformFeedbackBuffer";
		case BufferTarget::AtomicCounter:
			return "AtomicCounterBuffer";
		}

		return "Unknown";
	}

	std::string_view ToString(CompareFunction function) {
		switch (function) {
		case CompareFunction::Never:
			return "Never";
		case CompareFunction::Less:
			return "Less";
		case CompareFunction::LEqual:
			return "LessOrEqual";
		case CompareFunction::Greater:
			return "Greater";
		case CompareFunction::GEqual:
			return "GreaterOrEqual";
		case CompareFunction::Equal:
			return "Equal";
		case CompareFunction::NotEqual:
			return "NotEqual";
		case CompareFunction::Always:
			return "Always";
		}
		return "Unknown";
	}

	std::string_view ToString(UniformKind kind) {
		switch (kind) {
		case UniformKind::Scalar:
			return "Scalar";
		case UniformKind::Vector:
			return "Vector";
		case UniformKind::Matrix:
			return "Matrix";
		case UniformKind::Sampler:
			return "Sampler";
		case UniformKind::Image:
			return "Image";
		case UniformKind::AtomicCounter:
			return "AtomicCounter";

		default:
			return "Unknown";
		}
	}

	std::string_view ToString(ScalarKind kind) {
		switch (kind) {
		case ScalarKind::Bool:
			return "Bool";
		case ScalarKind::Int:
			return "Int";
		case ScalarKind::UInt:
			return "UInt";
		case ScalarKind::Float:
			return "Float";
		case ScalarKind::Double:
			return "Double";

		default:
			return "None";
		}
	}

	std::string_view ToString(TextureDim texture) {
		switch (texture) {
		case TextureDim::D1:
			return "1D";
		case TextureDim::D2:
			return "2D";
		case TextureDim::D3:
			return "3D";
		case TextureDim::Cube:
			return "Cube";
		case TextureDim::Rect:
			return "Rect";
		case TextureDim::Buffer:
			return "Buffer";
		case TextureDim::D1Array:
			return "1DArray";
		case TextureDim::D2Array:
			return "2DArray";
		case TextureDim::CubeArray:
			return "CubeArray";
		case TextureDim::D2MS:
			return "D2MS";
		case TextureDim::D2MSArray:
			return "D2MSArray";
		default:
			return "Unknown";
		}
	}

	std::string ToString(UniformTypeDesc desc) {
		using namespace std::string_literals;
		switch (desc.Kind) {
		case UniformKind::Scalar:
			return std::string(ToString(desc.Scalar));
		case UniformKind::Vector:
			return std::string(ToString(desc.Scalar)) + " vec" + std::to_string(desc.Rows);
		case UniformKind::Matrix:
			return std::string(ToString(desc.Scalar)) + " mat" + std::to_string(desc.Rows) + "x" + std::to_string(desc.Cols);
		case UniformKind::Sampler: {
			std::string base = "Sampler"s + std::string(ToString(desc.Dim));
			if (desc.Shadow) base += "Shadow";
			return base;
		}
		case UniformKind::Image:
			return "Image"s + std::string(ToString(desc.Dim));
		case UniformKind::AtomicCounter:
			return "Atomic_uInt";
		default:
			return "Unknown";
		}
	}
}
