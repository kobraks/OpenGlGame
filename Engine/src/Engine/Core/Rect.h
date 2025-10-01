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
		                                     Height(static_cast<T>(rect.Height)) {}

		[[nodiscard]] constexpr bool Contains(const Vector2<T>& point) const noexcept {
			return Contains(point.X, point.Y);
		}

		[[nodiscard]] constexpr bool Contains(ValueType x, ValueType y) const noexcept {
			const ValueType minX = std::min(X, X + Width);
			const ValueType maxX = std::max(X, X + Width);

			const ValueType minY = std::min(Y, Y + Height);
			const ValueType maxY = std::max(Y, Y + Height);

			return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
		}

		[[nodiscard]] constexpr Vector2<T> GetSize() const noexcept { return {Width, Height}; }
		[[nodiscard]] constexpr Vector2<T> GetPos() const noexcept { return {X, Y}; }

		[[nodiscard]] constexpr ValueType GetWidth() const noexcept { return Width; }
		[[nodiscard]] constexpr ValueType GetHeight() const noexcept { return Height; }

		[[nodiscard]] constexpr ValueType GetX() const noexcept { return X; }
		[[nodiscard]] constexpr ValueType GetY() const noexcept { return Y; }

		constexpr void SetSize(const Vector2<T> &size) noexcept {
			Width  = size.Width;
			Height = size.Height;
		}

		constexpr void SetPos(const Vector2<T> &pos) noexcept {
			X = pos.X;
			Y = pos.Y;
		}

		[[nodiscard]] constexpr Vector2<T> GetCenter() const noexcept {
			return { X + Width / static_cast<T>(2), Y + Height / static_cast<T>(2) };
		}

		[[nodiscard]] constexpr Rect<T> Normalized() const noexcept {
			T newX = X;
			T newY = Y;
			T newW = Width;
			T newH = Height;

			if (Width < T(0)) {
				newX += Width;
				newW = -Width;
			}
			if (Height < T(0)) {
				newY += Height;
				newH = -Height;
			}

			return Rect{ newX, newY, newW, newH };
		}

		constexpr void Offset(const Vector2<T>& offset) noexcept {
			X += offset.X;
			Y += offset.Y;
		}

		constexpr void Offset(ValueType dx, ValueType dy) noexcept {
			X += dx;
			Y += dy;
		}

		constexpr void Inflate(const Vector2<T>& amount) noexcept {
			X -= amount.X;
			Y -= amount.Y;
			Width += amount.X * static_cast<T>(2);
			Height += amount.Y * static_cast<T>(2);
		}

		constexpr void Inflate(ValueType dx, ValueType dy) noexcept {
			X -= dx;
			Y -= dy;
			Width += dx * static_cast<T>(2);
			Height += dy * static_cast<T>(2);
		}

		[[nodiscard]] constexpr std::optional<Rect<T>> FindIntersection(const Rect<T> &rect) const noexcept {
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

		[[nodiscard]] constexpr bool IsEmpty() const noexcept {
			return (Width <= T(0)) || (Height <= T(0));
		}

		[[nodiscard]] constexpr Vector2<T> GetMin() const noexcept {
			return { std::min(X, X + Width), std::min(Y, Y + Height) };
		}

		[[nodiscard]] constexpr Vector2<T> GetMax() const noexcept {
			return { std::max(X, X + Width), std::max(Y, Y + Height) };
		}

		template <typename R>
		explicit constexpr operator Rect<R>() const noexcept {
			return { static_cast<R>(X), static_cast<R>(Y), static_cast<R>(Width), static_cast<R>(Height) };
		} 
	};

	template <typename T>
	[[nodiscard]] constexpr bool operator==(const Rect<T> &lhs, const Rect<T> &rhs) noexcept {
		return (lhs.X == rhs.X) && (lhs.Y == rhs.Y) && (lhs.Width == rhs.Width) && (lhs.Height == rhs.Height);
	}

	template <typename T>
	[[nodiscard]] constexpr bool operator!=(const Rect<T> &lhs, const Rect<T> &rhs) noexcept {
		return !(lhs == rhs);
	}

	using UIntRect  = Rect<uint32_t>;
	using IntRect   = Rect<int>;
	using FloatRect = Rect<float>;
}
