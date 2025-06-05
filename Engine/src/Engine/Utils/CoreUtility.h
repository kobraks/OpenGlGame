#pragma once
#include "Engine/Core/Base.h"

namespace Engine::Utils {
	template<typename T>
	constexpr bool HasFlag(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
	}

	template<typename T>
	constexpr T SetFlag(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return static_cast<T>(static_cast<U>(value) | static_cast<U>(flag));
	}

	template<typename T>
	constexpr T ClearFlag(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return static_cast<T>(static_cast<U>(value) & ~static_cast<U>(flag));
	}

	template<typename T>
	constexpr bool IsAlignTo(const std::byte* ptr, uint64_t offset) {
		return reinterpret_cast<uintptr_t>(ptr + offset) % alignof(T) == 0;
	}

	template<typename T>
	constexpr T AlignDown(const T& value, size_t alignment) {
		return value & ~static_cast<T>(alignment - 1);
	}

	template <typename T>
	constexpr T AlignUp(const T& value, size_t alignment) {
		return (value + static_cast<T>(alignment - 1)) & ~static_cast<T>(alignment - 1);
	}
}