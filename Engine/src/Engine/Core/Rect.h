#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include <algorithm>
#include <optional>
#include <type_traits>
#include <bit>

#include <fmt/format.h>

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

		[[nodiscard]] static constexpr Rect FromMinMax(Vector2<T> min, Vector2<T> max) noexcept {
			return Rect(min.X, min.Y, max.X - min.X, max.Y - min.Y);
		}

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
			if constexpr (std::is_unsigned_v<T>)
				return *this;

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

		constexpr void Translate(T dx, T dy) noexcept {
			X += dx;
			Y += dy;
		}

		constexpr void Translate(const Vector2<T>& vector) noexcept {
			X += vector.X;
			Y += vector.Y;
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
			const auto intLeft   = std::max(Left(), rect.Left());
			const auto intTop    = std::max(Top(), rect.Top());
			const auto intRight  = std::min(Right(), rect.Right());
			const auto intBottom = std::min(Bottom(), rect.Bottom());

			if((intLeft < intRight) && (intTop < intBottom)) {
				return Rect<T>(intLeft, intTop, intRight - intLeft, intBottom - intTop);
			}

			return std::nullopt;
		}

		[[nodiscard]] constexpr bool Intersects(const Rect& r) const noexcept {
			const auto a = Normalized();
			const auto b = r.Normalized();
			return (a.X < b.X + b.Width) && (b.X < a.X + a.Width) && (a.Y < b.Y + b.Height) && (b.Y < a.Y + a.Height);
		}

		[[nodiscard]] constexpr bool IsEmpty() const noexcept {
			return (Width <= T(0)) || (Height <= T(0));
		}

		template <typename R>
		explicit constexpr operator Rect<R>() const noexcept {
			return { static_cast<R>(X), static_cast<R>(Y), static_cast<R>(Width), static_cast<R>(Height) };
		}

		[[nodiscard]] constexpr T Left() const noexcept { return std::min(X, X + Width); }
		[[nodiscard]] constexpr T Right() const noexcept { return std::max(X, X + Width); }
		[[nodiscard]] constexpr T Top() const noexcept { return std::min(Y, Y + Height); }
		[[nodiscard]] constexpr T Bottom() const noexcept { return std::max(Y, Y + Height); }
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

template <typename T>
struct fmt::formatter<Engine::Rect<T>> : fmt::nested_formatter<T> {
	enum class Style : uint8_t { Paren, Bracket, Json, WidthHeight, Descriptive };
	Style Mode = Style::Paren;

	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		auto it = ctx.begin();
		const auto end = ctx.end();

		if (it != end) {
			switch (*it) {
			case 'p': Mode = Style::Paren; ++it;  break;
			case 'b': Mode = Style::Bracket; ++it; break;
			case 'j': Mode = Style::Json; ++it; break;
			case 'w': Mode = Style::WidthHeight; ++it; break;
			case 'd': Mode = Style::Descriptive; ++it; break;
			default: break;
			}
		}

		if (it != end && *it == '|') { ++it; ctx.advance_to(it); return fmt::nested_formatter<T>::parse(ctx); }
		ctx.advance_to(it);
		return it;
	}

	template <typename FormatContext>
	auto format(const Engine::Rect<T>& rect, FormatContext& ctx) const {
		return fmt::nested_formatter<T>::write_padded(ctx, [this, &rect](auto out) {
			switch (Mode) {
			case Style::Paren: return fmt::format_to(out, "({}, {}) {}x{}", this->nested(rect.X), this->nested(rect.Y), this->nested(rect.Width), this->nested(rect.Height));
			case Style::Bracket: return fmt::format_to(out, "[{}, {}] {}x{}", this->nested(rect.X), this->nested(rect.Y), this->nested(rect.Width), this->nested(rect.Height));
			case Style::Json: return fmt::format_to(out, "{{\"x\": {}, \"y\": {}, \"width\": {}, \"height\": {}}}", this->nested(rect.X), this->nested(rect.Y), this->nested(rect.Width), this->nested(rect.Height));
			case Style::WidthHeight: return fmt::format_to(out, "{}x{}", this->nested(rect.Width), this->nested(rect.Height));
			case Style::Descriptive: return fmt::format_to(out, "Rect(x={}, y={}, w={}, h={})", this->nested(rect.X), this->nested(rect.Y), this->nested(rect.Width), this->nested(rect.Height));
			}
			});
	}
};

namespace std {
	template <typename T>
	struct hash<Engine::Rect<T>> {
		size_t operator()(const Engine::Rect<T>& rect) const noexcept {
			auto hash = Comp(rect.X);
			hash = Combine(hash, Comp(rect.Y));
			hash = Combine(hash, Comp(rect.Width));
			hash = Combine(hash, Comp(rect.Height));
			return hash;
		}

	private:
		static constexpr size_t Combine(size_t seed, size_t v) noexcept {
			if constexpr (sizeof(size_t) == 8)
				return seed ^ (v + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2));
			else
				return seed ^ (v + 0x9e3779b9u + (seed << 6) + (seed >> 2));
		}

		static constexpr size_t Comp(const T& v) noexcept {
			if constexpr (std::is_floating_point_v<T>) {
				T x = (v == T(0)) ? T(0) : v;
				if constexpr (std::is_same_v<T, float>) return std::hash<uint32_t>{}(std::bit_cast<uint32_t>(x));
				if constexpr (std::is_same_v<T, double>) return std::hash<uint64_t>{}(std::bit_cast<uint64_t>(x));
				return std::hash<long double>{}(static_cast<long double>(x));
			}
			else {
				return std::hash<T>{}(v);
			}
		}
	};
}
