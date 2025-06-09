#include "pch.h"
#include "StdUtils.h"

#include <unordered_map>

namespace Engine::Utils {
	const char* EnsureNullTerminated(std::string_view view, std::string& tmpStorage) {
		if (!view.empty() && view.back() == '\0') {
			return view.data(); // Already null-terminated
		}

		tmpStorage.assign(view);
		return tmpStorage.c_str(); // Ensure null-terminated
	}

	const char* EnsureNullTerminated(std::string_view view) {
		static thread_local std::string tmpStorage;

		return EnsureNullTerminated(view, tmpStorage);
	}
}
