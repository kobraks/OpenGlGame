#pragma once
#include "Engine/Renderer/BufferEnums.h"

namespace Engine {
	struct BufferUsageIntent {
		bool cpuWrite = true;
		bool cpuReads = false;
		bool gpuWrite = false;
		bool gpuReads = true;
		bool isDynamic = false;
		bool isStreaming = false;
	};

	BufferUsage BufferUsageStrategy(const BufferUsageIntent& intent);
}

namespace Engine::Utils {
	constexpr BufferAccess DetermineAccessFromFlags(BufferStorageFlags flags) {
		if (HasFlag(flags, BufferStorageFlags::MapRead) && HasFlag(flags, BufferStorageFlags::MapWrite))
			return BufferAccess::ReadWrite;
		if (HasFlag(flags, BufferStorageFlags::MapRead))
			return BufferAccess::ReadOnly;
		if (HasFlag(flags, BufferStorageFlags::MapWrite))
			return BufferAccess::WriteOnly;

		ENGINE_ASSERT(false, "Invalid mapping flags for determining BufferAccess.");
		throw std::runtime_error("BufferStorageFlags must include MapRead or MapWrite to determine access.");
	}

	inline void AssertAccess(BufferAccess access, bool requiresRead, bool requiresWrite) {
		if (requiresRead && access == BufferAccess::WriteOnly) {
			ENGINE_ASSERT(false, "Attempted to read form buffer mapped as WriteOnly");
			throw std::runtime_error("Attempted to read from buffer mapped as WriteOnly");
		}

		if (requiresWrite && access == BufferAccess::ReadOnly) {
			ENGINE_ASSERT(false, "Attempted to write into buffer mapped as ReadOnly");
			throw std::runtime_error("Attempted to write into buffer mapped as ReadOnly");
		}
	}
}