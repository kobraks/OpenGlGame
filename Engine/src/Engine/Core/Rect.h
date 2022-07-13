#pragma once
#include "Engine/Core/Vector2.h"

namespace Game
{
	template <typename Type>
	class Rect
	{
	public:
		using ValueType = Type;

		ValueType X;
		ValueType Y;
		ValueType Width;
		ValueType Height;

		constexpr Rect() : X(ValueType{}),
		                   Y(ValueType{}),
		                   Width(ValueType{}),
		                   Height(ValueType{}) {}

		constexpr Rect(ValueType x, ValueType y, ValueType width, ValueType height) : X(x),
		                                                                              Y(y),
		                                                                              Width(width),
		                                                                              Height(height) {}

		constexpr Rect(const Vector2<Type> &position, const Vector2<Type> &size) : X(position.X),
		                                                                           Y(position.Y),
		                                                                           Width(size.Width),
		                                                                           Height(size.Height) {}

		template <typename U>
		explicit Rect(const Rect<U> &rect) : X(static_cast<Type>(rect.X)),
		                                     Y(static_cast<Type>(rect.Y)),
		                                     Width(static_cast<Type>(rect.Width)),
		                                     Height(static_cast<Type>(rect.Height)) {}

		constexpr bool Contains(ValueType x, ValueType y) const
		{
			const ValueType minX = std::min(X, static_cast<ValueType>(X + Width));
			const ValueType maxX = std::max(X, static_cast<ValueType>(X + Width));

			const ValueType minY = std::min(Y, static_cast<ValueType>(Y + Height));
			const ValueType maxY = std::max(Y, static_cast<ValueType>(Y + Height));

			return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
		}

		constexpr bool Contains(const Vector2<Type> &point) const
		{
			return Contains(point.X, point.Y);
		}

		constexpr bool Intersects(const Rect<Type> &rectangle) const
		{
			Rect<Type> intersection;
			return Intersects(rectangle, intersection);
		}

		constexpr bool Intersects(const Rect<Type> &rectangle, Rect<Type> &intersection) const
		{
			const Type r1MinX = std::min(X, static_cast<Type>(X + Width));
			const Type r1MaxX = std::max(X, static_cast<Type>(X + Width));
			const Type r1MinY = std::min(Y, static_cast<Type>(Y + Height));
			const Type r1MaxY = std::min(Y, static_cast<Type>(Y + Height));

			const Type r2MinX = std::min(rectangle.X, static_cast<Type>(rectangle.X + rectangle.Width));
			const Type r2MaxX = std::max(rectangle.X, static_cast<Type>(rectangle.X + rectangle.Width));
			const Type r2MinY = std::min(rectangle.Y, static_cast<Type>(rectangle.Y + rectangle.Height));
			const Type r2MaxY = std::min(rectangle.Y, static_cast<Type>(rectangle.Y + rectangle.Height));

			const Type interLeft   = std::max(r1MinX, r2MinX);
			const Type interTop    = std::max(r1MinY, r2MinY);
			const Type interRight  = std::min(r1MaxX, r2MaxX);
			const Type interBottom = std::min(r1MaxY, r2MaxY);

			if((interLeft < interRight) && (interTop < interBottom))
			{
				intersection = Rect<Type>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
				return true;
			}

			intersection = Rect<Type>(0, 0, 0, 0);
			return false;
		}
	};

	template<typename Type>
	constexpr bool operator==(const Rect<Type>& left, const Rect<Type>& right)
	{
		return (left.X == right.X) && (left.Width == right.Width) && (left.Y == right.Y) && (left.Height == right.Height); 
	}
	
	template<typename Type>
	constexpr bool operator!=(const Rect<Type>& left, const Rect<Type>& right)
	{
		return !(left == right);
	}

	typedef Rect<uint32_t> UIntRect;
	typedef Rect<int> IntRect;
	typedef Rect<float> FloatRect;
}
