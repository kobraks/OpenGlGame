#include "pch.h"
#include "StdUtils.h"

#include <array>

namespace Engine::Utils {
	constexpr std::size_t s_BufferSize = 15;

	const char* EnsureNullTerminated(std::string_view view, std::string& tmpStorage) {
		tmpStorage.assign(view);
		return tmpStorage.c_str(); // Ensure null-terminated
	}

	const char* EnsureNullTerminated(std::string_view view) {
		static thread_local std::size_t index = 0;
		static thread_local std::array<std::string, s_BufferSize> tmpStorage;

		std::string& current = tmpStorage[index];
		index = (index + 1) % s_BufferSize;

		return EnsureNullTerminated(view, current);
	}
}
