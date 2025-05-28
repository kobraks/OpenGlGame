#pragma once
#include "Engine/Core/Base.h"

namespace Engine::Utils {
	template<typename T>
	bool IsAlignTo(const std::byte* ptr, uint64_t offset) {
		return reinterpret_cast<uintptr_t>(ptr + offset) % alignof(T) == 0;
	}
}