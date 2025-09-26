#pragma once

#include "Engine/Core/Base.h"

#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <tuple>

#include <glm/vec2.hpp>
#include <fmt/format.h>

namespace Engine {
	template <typename T>
	class Vector2 {
	public:
		using ValueType = T;

		static_assert(std::is_arithmetic_v<T>, "Vector2 requires an arithmetic type (int, float, etc).");

		union {
			ValueType X;
			ValueType Width;
		};

		union {
			ValueType Y;
			ValueType Height;
		};


		constexpr Vector2(ValueType x, ValueType y) noexcept : X{x}, Y{y} {}
		constexpr explicit Vector2(const ValueType scalar) noexcept : Vector2(scalar, scalar) {}
		constexpr Vector2() noexcept : X{}, Y{} {}

		template<typename R, glm::qualifier Q>
			requires (std::is_floating_point_v<T>&& std::is_floating_point_v<R>)
		constexpr explicit Vector2(const glm::vec<2, R, Q>& v) noexcept : Vector2(static_cast<T>(v.x), static_cast<T>(v.y)) {}

		[[nodiscard]] constexpr bool operator==(const Vector2<T> &vec) const noexcept {
			return X == vec.X && Y == vec.Y;
		}

		[[nodiscard]] constexpr bool operator!=(const Vector2<T> &vec) const noexcept {
			return !(*this == vec);
		}

		template<typename R, glm::qualifier Q>
			requires (std::is_floating_point_v<T>&& std::is_floating_point_v<R>)
		Vector2& operator=(const glm::vec<2, R, Q>& v) noexcept {
			X = static_cast<T>(v.x);
			Y = static_cast<T>(v.y);
			return *this;
		}

		template<typename R, glm::qualifier Q>
			requires (std::is_floating_point_v<T>&& std::is_floating_point_v<R>)
		[[nodiscard]] bool operator==(const glm::vec<2, R, Q>& v) const noexcept {
			return X == static_cast<T>(v.x) && Y == static_cast<T>(v.y);
		}

		template<typename R, glm::qualifier Q>
			requires (std::is_floating_point_v<T>&& std::is_floating_point_v<R>)
		[[nodiscard]] bool operator!=(const glm::vec<2, R, Q>& v) const noexcept {
			return !(*this == v);
		}

		template<typename R, glm::qualifier Q>
			requires (std::is_floating_point_v<T>&& std::is_floating_point_v<R>)
		constexpr explicit operator glm::vec<2, R, Q>() const noexcept {
			return { static_cast<R>(X), static_cast<R>(Y) };
		}

		template<typename R>
		constexpr explicit operator Vector2<R>() const noexcept {
			return { static_cast<R>(X), static_cast<R>(Y) };
		}

		constexpr auto operator[](std::ptrdiff_t i) -> ValueType& {
			switch(i) {
				case 0: return X;
				case 1: return Y;
				default: throw std::out_of_range("Vector2 index out of range (expected 0 or 1)");
			}
		}

		constexpr auto operator[](std::ptrdiff_t i) const -> ValueType const& {
			switch(i) {
				case 0: return X;
				case 1: return Y;
				default: throw std::out_of_range("Vector2 index out of range (expected 0 or 1)");
			}
		}

		constexpr void Clamp(ValueType minVal, ValueType maxVal) noexcept {
			X = std::clamp(X, minVal, maxVal);
			Y = std::clamp(Y, minVal, maxVal);
		}

		constexpr void Clamp(const Vector2& minVec, const Vector2& maxVec) noexcept {
			X = std::clamp(X, minVec.X, maxVec.X);
			Y = std::clamp(Y, minVec.Y, maxVec.Y);
		}

		constexpr void ClampToBounds(const Vector2& minVec, const Vector2& maxVec) noexcept {
			Clamp(minVec, maxVec);
		}

		[[nodiscard]] constexpr Vector2 Clamped(ValueType minVal, ValueType maxVal) const noexcept {
			return { std::clamp(X, minVal, maxVal), std::clamp(Y, minVal, maxVal) };
		}
		[[nodiscard]] constexpr Vector2 Clamped(const Vector2& minVec, const Vector2& maxVec) const noexcept {
			return { std::clamp(X, minVec.X, maxVec.X), std::clamp(Y, minVec.Y, maxVec.Y) };
		}

		[[nodiscard]] constexpr Vector2 Min(const Vector2& other) const noexcept {
			return { std::min(X, other.X), std::min(Y, other.Y) };
		}

		[[nodiscard]] constexpr Vector2 Max(const Vector2& other) const noexcept {
			return { std::max(X, other.X), std::max(Y, other.Y) };
		}

		[[nodiscard]] constexpr static Vector2 Min(const Vector2& a, const Vector2& b) noexcept {
			return { std::min(a.X, b.X), std::min(a.Y, b.Y) };
		}

		[[nodiscard]] constexpr static Vector2 Max(const Vector2& a, const Vector2& b) noexcept {
			return { std::max(a.X, b.X), std::max(a.Y, b.Y) };
		}
	};

	typedef Vector2<uint32_t> Vector2u;
	typedef Vector2<uint64_t> Vector2ul;

	typedef Vector2<int32_t> Vector2i;
	typedef Vector2<int64_t> Vector2il;

	typedef Vector2<float> Vector2f;
	typedef Vector2<double> Vector2d;
}


//Structured binding: auto [x, y] = vec;
namespace std {
	template<typename T>
	struct tuple_size<Engine::Vector2<T>> : std::integral_constant<std::size_t, 2> {};

	template<typename T>
	struct tuple_element<0, Engine::Vector2<T>> {
		using type = T;
	};

	template<typename T>
	struct tuple_element<1, Engine::Vector2<T>> {
		using type = T;
	};
}

namespace Engine {
	template<std::size_t I, typename T>
	constexpr T& get(Vector2<T>& v) noexcept {
		static_assert(I < 2, "Index out of bounds for Vector2");

		if constexpr (I == 0) return v.X;
		else return v.Y;
	}

	template<std::size_t I, typename T>
	constexpr const T& get(const Vector2<T>& v) noexcept {
		static_assert(I < 2, "Index out of bounds for Vector2");

		if constexpr (I == 0) return v.X;
		else return v.Y;
	}

	template<std::size_t I, typename T>
	constexpr T&& get(Vector2<T>&& v) noexcept {
		static_assert(I < 2, "Index out of bounds for Vector2");
		if constexpr (I == 0) return std::move(v.X);
		else return std::move(v.Y);
	}
}

#define ENGINE_VECTOR2_FMT_IMPL(type)\
template<>\
struct fmt::formatter<Engine::type>: public fmt::nested_formatter<Engine::type::ValueType> { \
	auto format(const Engine::type &vec, format_context& ctx) const { \
		return write_padded(ctx, [=, this](auto out) { \
			return format_to(out, "({}, {})", nested(vec.X), nested(vec.Y)); \
		}); \
	} \
};

ENGINE_VECTOR2_FMT_IMPL(Vector2u);
ENGINE_VECTOR2_FMT_IMPL(Vector2ul);
ENGINE_VECTOR2_FMT_IMPL(Vector2i);
ENGINE_VECTOR2_FMT_IMPL(Vector2il);
ENGINE_VECTOR2_FMT_IMPL(Vector2f);
ENGINE_VECTOR2_FMT_IMPL(Vector2d);

namespace std {
	//Hash support
	template<typename T>
	struct hash<Engine::Vector2<T>> {
		size_t operator()(const Engine::Vector2<T>& v) const noexcept {
			size_t h1 = std::hash<T>{}(v.X);
			size_t h2 = std::hash<T>{}(v.Y);

			//Combine hashes
			return h1 ^ (h2 << 1);
		}
	};
}