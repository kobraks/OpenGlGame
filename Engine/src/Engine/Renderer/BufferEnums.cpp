#include "pch.h"
#include "BufferEnums.h"

#include <glad/glad.h>

namespace Engine {
	namespace Utils {
		const char* ToString(BufferStorageFlags flags) {
			if (flags == BufferStorageFlags::None)
				return "None";
			static std::string result;
			result.clear();
			if (HasFlag(flags, BufferStorageFlags::Dynamic)) result += "Dynamic | ";
			if (HasFlag(flags, BufferStorageFlags::MapRead)) result += "MapRead | ";
			if (HasFlag(flags, BufferStorageFlags::MapWrite)) result += "MapWrite | ";
			if (HasFlag(flags, BufferStorageFlags::MapPersistent)) result += "MapPersistent | ";
			if (HasFlag(flags, BufferStorageFlags::MapCoherent)) result += "MapCoherent | ";
			if (HasFlag(flags, BufferStorageFlags::ClientStorage)) result += "ClientStorage | ";

			if (!result.empty())
				result.pop_back(), result.pop_back(); // Remove trailing " | "

			return result.c_str();
		}

		const char* ToString(BufferStorageMode mode) {
			switch (mode) {
			case BufferStorageMode::Immutable:
				return "Immutable";
			case BufferStorageMode::Mutable:
				return "Mutable";
			}
			return "Unknown";
		}

		const char* ToString(BufferAccess access) {
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

		const char* ToString(BufferTarget target) {
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

		uint32_t ToGL(BufferAccess access) {
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

		uint32_t ToGL(BufferStorageFlags flags) {
			uint32_t glFlags = 0;

			if (HasFlag(flags, BufferStorageFlags::Dynamic)) glFlags |= GL_DYNAMIC_STORAGE_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapRead)) glFlags |= GL_MAP_READ_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapWrite)) glFlags |= GL_MAP_WRITE_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapPersistent)) glFlags |= GL_MAP_PERSISTENT_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapCoherent)) glFlags |= GL_MAP_COHERENT_BIT;
			if (HasFlag(flags, BufferStorageFlags::ClientStorage)) glFlags |= GL_CLIENT_STORAGE_BIT;

			return glFlags;
		}

		uint32_t ToGL(BufferTarget target) {
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
	}
}