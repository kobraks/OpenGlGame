#pragma once

#include <cstdint>

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

		Vector2(const ValueType x, const ValueType Y) : X(x),
		                                                Y(y) {}

		Vector2(const ValueType value) : Vector2(value, value) {}

		Vector2() : X({}),
		            Y({}) {}

		bool operator==(const Vector2<Type>& vector) const
		{
			return X == vector.X && Y == vector.Y;
		}
		
		bool operator!=(const Vector2<Type>& vector) const
		{
			return !operator==(vector);
		}
	};

	typedef Vector2<uint32_t> Vector2u;
	typedef Vector2<int32_t> Vector2i;
	typedef Vector2<float> Vector2f;
}
