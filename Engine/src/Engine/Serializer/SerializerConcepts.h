#pragma once

#include <concepts>
#include <type_traits>

namespace Engine::Serialization {
	template <typename T>
	concept Integer = std::integral<std::remove_cvref_t<T>>;

	template <typename T>
	concept FloatingPoint = std::floating_point<std::remove_cvref_t<T>>;

	template <typename T>
	concept Scalar = Integer<T> || FloatingPoint<T>;

	template <typename T>
	concept Enum = std::is_enum_v<std::remove_cvref_t<T>>;

	template <typename T>
	concept RawCopyable = std::is_trivially_copyable_v < std::remove_cvref_t<T>>;
}