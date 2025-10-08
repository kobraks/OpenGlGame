#pragma once
#include "Engine/Core/Base.h"

#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <algorithm>

namespace Engine::Utils {
	template<typename T>
	constexpr bool HasFlag(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
	}

	template<typename T>
	constexpr T& SetFlag(T& value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return value = static_cast<T>(static_cast<U>(value) | static_cast<U>(flag));
	}

	template <typename T>
	constexpr T SetFlagCopy(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return static_cast<T>(static_cast<U>(value) | static_cast<U>(flag));
	}

	template<typename T>
	constexpr T& ClearFlag(T& value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return value = static_cast<T>(static_cast<U>(value) & ~static_cast<U>(flag));
	}

	template <typename T>
	constexpr T ClearFlagCopy(T value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return static_cast<T>(static_cast<U>(value) & ~static_cast<U>(flag));
	}

	template <typename T>
	constexpr T FlipFlag(T& value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return value = static_cast<T>(static_cast<U>(value) ^ static_cast<U>(flag));
	}

	template <typename T>
	constexpr T FlipFlagCopy(T& value, T flag) {
		using U = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;
		return static_cast<T>(static_cast<U>(value) ^ static_cast<U>(flag));
	}

	template<typename T>
	constexpr bool IsAlignTo(const std::byte* ptr, uint64_t offset) {
		return reinterpret_cast<uintptr_t>(ptr + offset) % alignof(T) == 0;
	}

	template<typename T>
	requires std::is_integral_v<T>
	constexpr T AlignDown(const T& value, size_t alignment) {
		return value & ~static_cast<T>(alignment - 1);
	}

	template <typename T>
	requires std::is_integral_v<T>
	constexpr T AlignUp(const T& value, size_t alignment) {
		return (value + static_cast<T>(alignment - 1)) & ~static_cast<T>(alignment - 1);
	}

	constexpr std::string_view GetFirst(const std::string& string, std::size_t n) {
		return { string.data(), std::min(n, string.size())};
	}
}