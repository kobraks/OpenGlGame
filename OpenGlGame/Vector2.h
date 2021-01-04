#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <fmt/format.h>

namespace Game
{
	template <typename Type>
	class Vector2
	{
	public:
		using ValueType = Type;

		union
		{
			struct
			{
				ValueType X;
				ValueType Y;
			};

			struct
			{
				ValueType Width;
				ValueType Height;
			};
		};

		template <typename = std::enable_if<std::is_same_v<float, Type>>>
		Vector2(const glm::vec2 v) : X(v.x),
		                             Y(v.y) {}

		constexpr Vector2(const ValueType x, const ValueType y) : X(x),
		                                                          Y(y) {}

		constexpr Vector2(const ValueType value) : Vector2(value, value) {}

		constexpr Vector2() : X(ValueType{}),
		                      Y(ValueType{}) {}

		constexpr bool operator==(const Vector2<Type> &vector) const
		{
			return X == vector.X && Y == vector.Y;
		}

		constexpr bool operator!=(const Vector2<Type> &vector) const
		{
			return !operator==(vector);
		}

		template <typename = std::enable_if<std::is_same_v<float, Type>>>
		Vector2& operator=(const glm::vec2 v)
		{
			X = v.x;
			Y = v.y;
		}
	};

	typedef Vector2<uint32_t> Vector2u;
	typedef Vector2<uint64_t> Vector2ul;
	typedef Vector2<int32_t> Vector2i;
	typedef Vector2<int64_t> Vector2il;
	typedef Vector2<float> Vector2f;
	typedef Vector2<double> Vector2d;
}

#define VECTOR2_FMT(Type) \
	template<>\
	struct fmt::formatter<Game::Type> : public fmt::formatter<Game::Type::ValueType>\
	{\
		template <typename FormatContext>\
		auto format(const Game::Type &vector, FormatContext &context)\
		{\
			fmt::format_to(context.out(), "(");\
			fmt::format_to(fmt::formatter<Game::Type::ValueType>::format(vector.X, context), ", ");\
			fmt::formatter<Game::Type::ValueType>::format(vector.Y, context); \
			fmt::format_to(context.out(), ")");\
			\
			return context.out();\
		}\
	};

VECTOR2_FMT(Vector2u);
VECTOR2_FMT(Vector2ul);
VECTOR2_FMT(Vector2i);
VECTOR2_FMT(Vector2il);
VECTOR2_FMT(Vector2f);
VECTOR2_FMT(Vector2d);