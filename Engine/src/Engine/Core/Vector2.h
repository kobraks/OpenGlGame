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

		template <typename = std::enable_if<std::is_same_v<float, T>>>
		Vector2(const glm::vec2 v) : Vector2(v.x, v.y) {}

		constexpr bool operator==(const Vector2<T> &vec) const {
			return X == vec.X && Y == vec.Y;
		}

		constexpr bool operator!=(const Vector2<T> &vec) const {
			return !(*this == vec);
		}

		template<typename = std::enable_if<std::is_same_v<float, T>>>
		Vector2& operator=(const glm::vec2 v) {
			X = v.x;
			Y = v.y;

			return *this;
		}

		constexpr auto operator[](std::ptrdiff_t i) -> ValueType& {
			switch(i) {
				case 0: return X;
				case 1: return Y;
				default: throw std::out_of_range("");
			}
		}

		constexpr auto operator[](std::ptrdiff_t i) const -> ValueType const& {
			switch(i) {
				case 0: return X;
				case 1: return Y;
				default: throw std::out_of_range("");
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

#define ENGINE_VECTOR2_FMT(type)\
template<>\
struct fmt::formatter<Engine::type>: public fmt::nested_formatter<Engine::type::ValueType> { \
	auto format(const Engine::type &vec, format_context& ctx) const { \
		return write_padded(ctx, [=, this](auto out) { \
			return format_to(out, "({}, {})", nested(vec.X), nested(vec.Y)); \
		}); \
	} \
};

ENGINE_VECTOR2_FMT(Vector2u);
ENGINE_VECTOR2_FMT(Vector2ul);
ENGINE_VECTOR2_FMT(Vector2i);
ENGINE_VECTOR2_FMT(Vector2il);
ENGINE_VECTOR2_FMT(Vector2f);
ENGINE_VECTOR2_FMT(Vector2d);