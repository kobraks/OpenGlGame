#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include <algorithm>
#include <optional>

namespace Engine {
	template <typename T>
	class Rect {
	public:
		using ValueType = T;

		ValueType X;
		ValueType Y;
		ValueType Width;
		ValueType Height;

		constexpr Rect() : X{},
		                   Y{},
		                   Width{},
		                   Height{} {}

		constexpr Rect(ValueType x, ValueType y, ValueType width, ValueType height) : X(x),
			Y(y),
			Width(width),
			Height(height) {}

		constexpr Rect(const Vector2<T> &pos, const Vector2<T> &size) : X(pos.X),
		                                                                Y(pos.Y),
		                                                                Width(size.Width),
		                                                                Height(size.Height) {}

		template <typename U>
		explicit Rect(const Rect<U> &rect) : X(static_cast<T>(rect.X)),
		                                     Y(static_cast<T>(rect.Y)),
		                                     Width(static_cast<T>(rect.Width)),
		                                     Height(static_cast<T>(rect.Hegiht)) {}

		constexpr bool Contains(ValueType x, ValueType y) const {
			const ValueType minX = std::min(X, X + Width);
			const ValueType maxX = std::max(X, X + Width);

			const ValueType minY = std::min(Y, Y + Height);
			const ValueType maxY = std::max(Y, Y + Height);

			return (X >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
		}

		Vector2<T> GetSize() const { return {Width, Height}; }
		Vector2<T> GetPos() const { return {X, Y}; }

		void SetSize(const Vector2<T> &size) {
			Width  = size.Width;
			Height = size.Height;
		}

		void SetPos(const Vector2<T> &pos) {
			X = pos.X;
			Y = pos.Y;
		}

		constexpr bool Contains(const Vector2<T> &point) {
			return Contains(point.X, point.Y);
		}

		constexpr std::optional<Rect<T>> FindIntersection(const Rect<T> &rect) const {
			const auto r1MinX = std::min(X, X + Width);
			const auto r1MaxX = std::max(X, X + Width);
			const auto r1MinY = std::min(Y, Y + Height);
			const auto r1MaxY = std::max(Y, Y + Height);

			const auto r2MinX = std::min(rect.X, rect.X + rect.Width);
			const auto r2MaxX = std::max(rect.X, rect.X + rect.Width);
			const auto r2MinY = std::min(rect.Y, rect.Y + rect.Height);
			const auto r2MaxY = std::max(rect.Y, rect.Y + rect.Height);

			const auto intLeft   = std::max(r1MinX, r2MinX);
			const auto intTop    = std::max(r1MinY, r2MinY);
			const auto intRight  = std::min(r1MaxX, r2MaxX);
			const auto intBottom = std::min(r1MaxY, r2MaxY);

			if((intLeft < intRight) && (intTop < intBottom)) {
				return Rect<T>(intLeft, intTop, intRight - intLeft, intBottom - intTop);
			}

			return std::nullopt;
		}
	};

	template <typename T>
	constexpr bool operator==(const Rect<T> &lth, const Rect<T> &rth) {
		return (lth.X == rth.X) && (lth.Y == rth.Y) && (lth.Width == rth.Width) && (lth.Height == rth.Height);
	}

	template <typename T>
	constexpr bool operator!=(const Rect<T> &lth, const Rect<T> &rth) {
		return !(lth == rth);
	}

	using UIntRect  = Rect<uint32_t>;
	using IntRect   = Rect<int>;
	using FloatRect = Rect<float>;
}
