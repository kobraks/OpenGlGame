#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

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

		template<typename = std::enable_if<std::is_same_v<float, Type>>>
		Vector2(const glm::vec2 v) : X(v.x), Y(v.y)
		{}

		Vector2(const ValueType x, const ValueType y) : X(x),
		                                                Y(y) {}

		Vector2(const ValueType value) : Vector2(value, value) {}

		Vector2() : X(ValueType{}),
		            Y(ValueType{}) {}

		bool operator==(const Vector2<Type>& vector) const
		{
			return X == vector.X && Y == vector.Y;
		}
		
		bool operator!=(const Vector2<Type>& vector) const
		{
			return !operator==(vector);
		}

		template<typename = std::enable_if<std::is_same_v<float, Type>>>
		Vector2& operator=(const glm::vec2 v)
		{
			X = v.x;
			Y = v.y;
		}
	};

	typedef Vector2<uint32_t> Vector2u;
	typedef Vector2<int32_t> Vector2i;
	typedef Vector2<float> Vector2f;
}
