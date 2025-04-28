#pragma once

#include "Engine/Core/Base.h"

#include <cstdint>
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


		constexpr Vector2(ValueType x, ValueType y) : X{x}, Y{y} {} 
		constexpr Vector2(const ValueType scalar) : Vector2(scalar, scalar) {}
		constexpr Vector2() : X{}, Y{} {}

		//template <typename = std::enable_if_t<std::is_floating_point_v<T>>>
		Vector2(const glm::vec2& v) requires std::is_floating_point_v<T> : Vector2(v.x, v.y) {}

		constexpr bool operator==(const Vector2<T> &vec) const {
			return X == vec.X && Y == vec.Y;
		}

		constexpr bool operator!=(const Vector2<T> &vec) const {
			return !(*this == vec);
		}

		//template<typename = std::enable_if_t<std::is_floating_point_v<T>>>
		Vector2& operator=(const glm::vec2& v) requires std::is_floating_point_v<T> {
			X = v.x;
			Y = v.y;

			return *this;
		}

		template<typename R>
		constexpr operator Vector2<R>() const {
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