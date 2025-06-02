#include "pch.h"
#include "GLEnumConverters.h"
#include <glad/glad.h>

namespace Engine::Utils {
	uint32_t EnumToGLConstant(ImageFormat format) {
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

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(FilterMode filter) {
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

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(WrapMode wrapMode) {
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

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(DataFormat format) {
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

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(DataType type) {
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

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(BlitFilter filter) {
		switch (filter) {
		case BlitFilter::Nearest:
			return GL_NEAREST;
		case BlitFilter::Linear:
			return GL_LINEAR;
		}

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(BlitMask mask) {
		switch (mask) {
		case BlitMask::Color:
			return GL_COLOR_BUFFER_BIT;
		case BlitMask::Depth:
			return GL_DEPTH_BUFFER_BIT;
		case BlitMask::ColorAndDepth:
			return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		}

		ENGINE_ASSERT(false);
		throw std::out_of_range("");
	}

	uint32_t EnumToGLConstant(BufferAccess access) {
		switch (access) {
		case BufferAccess::ReadOnly:
			return GL_READ_ONLY;
		case BufferAccess::WriteOnly:
			return GL_WRITE_ONLY;
		case BufferAccess::ReadWrite:
			return GL_READ_WRITE;
		}

		return GL_READ_WRITE;
	}

	uint32_t EnumToGLConstant(BufferStorageFlags flags) {
		uint32_t glFlags = 0;

		if (HasFlag(flags, BufferStorageFlags::Dynamic)) glFlags |= GL_DYNAMIC_STORAGE_BIT;
		if (HasFlag(flags, BufferStorageFlags::MapRead)) glFlags |= GL_MAP_READ_BIT;
		if (HasFlag(flags, BufferStorageFlags::MapWrite)) glFlags |= GL_MAP_WRITE_BIT;
		if (HasFlag(flags, BufferStorageFlags::MapPersistent)) glFlags |= GL_MAP_PERSISTENT_BIT;
		if (HasFlag(flags, BufferStorageFlags::MapCoherent)) glFlags |= GL_MAP_COHERENT_BIT;
		if (HasFlag(flags, BufferStorageFlags::ClientStorage)) glFlags |= GL_CLIENT_STORAGE_BIT;

		return glFlags;
	}

	uint32_t EnumToGLConstant(BufferTarget target) {
		switch (target) {
		case BufferTarget::Array:
			return GL_ARRAY_BUFFER;
		case BufferTarget::ElementArray:
			return GL_ELEMENT_ARRAY_BUFFER;
		case BufferTarget::Uniform:
			return GL_UNIFORM_BUFFER;
		case BufferTarget::ShaderStorage:
			return GL_SHADER_STORAGE_BUFFER;
		case BufferTarget::CopyRead:
			return GL_COPY_READ_BUFFER;
		case BufferTarget::CopyWrite:
			return GL_COPY_WRITE_BUFFER;
		case BufferTarget::PixelPack:
			return GL_PIXEL_PACK_BUFFER;
		case BufferTarget::PixelUnpack:
			return GL_PIXEL_UNPACK_BUFFER;
		case BufferTarget::DrawIndirect:
			return GL_DRAW_INDIRECT_BUFFER;
		case BufferTarget::DispatchIndirect:
			return GL_DISPATCH_INDIRECT_BUFFER;
		case BufferTarget::TransformFeedback:
			return GL_TRANSFORM_FEEDBACK_BUFFER;
		case BufferTarget::AtomicCounter:
			return GL_ATOMIC_COUNTER_BUFFER;
		}
		return 0; // Invalid target
	}

	uint32_t EnumToGLConstant(BufferUsage usage) {
		switch (usage) {
		case BufferUsage::StaticDraw: return GL_STATIC_DRAW;
		case BufferUsage::StaticRead: return GL_STATIC_READ;
		case BufferUsage::StaticCopy: return GL_STATIC_COPY;
		case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
		case BufferUsage::DynamicRead: return GL_DYNAMIC_READ;
		case BufferUsage::DynamicCopy: return GL_DYNAMIC_COPY;
		case BufferUsage::StreamDraw: return GL_STREAM_DRAW;
		case BufferUsage::StreamRead: return GL_STREAM_READ;
		case BufferUsage::StreamCopy: return GL_STREAM_COPY;
		}
	}
}