#pragma once
#include "Engine/Core/Base.h"

#include <type_traits>

namespace Engine {
	template <typename T>
	class Flags {
		static_assert(std::is_enum_v<T>, "Template parameter 'Enum' must be an enum type.");

	public:
		using UnderlyingType = std::underlying_type_t<T>;

		constexpr Flags() = default;
		constexpr explicit Flags(T e) : m_Value(static_cast<UnderlyingType>(e)) {}
		constexpr explicit Flags(UnderlyingType e) : m_Value(e) {}

		constexpr bool operator==(T e) const noexcept { return m_Value == static_cast<UnderlyingType>(e); }
		constexpr bool operator!=(T e) const noexcept { return !(*this == e); }

		[[nodiscard]] explicit operator T() const noexcept { return static_cast<T>(m_Value); }
		[[nodiscard]] explicit operator UnderlyingType() const noexcept { return m_Value; }

		[[nodiscard]] constexpr bool HasAll(T e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) == static_cast<UnderlyingType>(e); }
		[[nodiscard]] constexpr bool HasAny(T e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) != 0; }
		[[nodiscard]] constexpr bool HasNone(T e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) == 0; }
		[[nodiscard]] constexpr bool IsNone() const noexcept { return m_Value == 0; }

		constexpr void Disable(T e) noexcept { m_Value &= ~static_cast<UnderlyingType>(e); }
		constexpr void Enable(T e) noexcept { m_Value |= static_cast<UnderlyingType>(e); }
		constexpr void Toggle(T e) noexcept { m_Value ^= static_cast<UnderlyingType>(e); }
		constexpr void Set(T e, bool enabled) noexcept {
			if (enabled) m_Value |= static_cast<UnderlyingType>(e);
			else m_Value &= ~static_cast<UnderlyingType>(e);
		}
		constexpr void Clear() noexcept { m_Value = 0; }

		[[nodiscard]] constexpr T Get() const noexcept { return static_cast<T>(m_Value); }
		[[nodiscard]] constexpr UnderlyingType Raw() const noexcept { return m_Value; }

		friend constexpr Flags operator | (Flags a, Flags b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value | b.m_Value) }; }
		friend constexpr Flags operator & (Flags a, Flags b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value & b.m_Value) }; }
		friend constexpr Flags operator ^ (Flags a, Flags b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value ^ b.m_Value) }; }

		friend constexpr Flags operator | (Flags a, T b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value | static_cast<UnderlyingType>(b)) }; }
		friend constexpr Flags operator & (Flags a, T b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value & static_cast<UnderlyingType>(b)) }; }
		friend constexpr Flags operator ^ (Flags a, T b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value ^ static_cast<UnderlyingType>(b)) }; }

		friend constexpr Flags operator | (T a, Flags b) noexcept { return b | a; }
		friend constexpr Flags operator & (T a, Flags b) noexcept { return b & a; }
		friend constexpr Flags operator ^ (T a, Flags b) noexcept { return b ^ a; }

		constexpr Flags& operator|=(Flags rhs) noexcept { m_Value |= rhs.m_Value; return *this; }
		constexpr Flags& operator&=(Flags rhs) noexcept { m_Value &= rhs.m_Value; return *this; }
		constexpr Flags& operator^=(Flags rhs) noexcept { m_Value ^= rhs.m_Value; return *this; }

		constexpr Flags& operator|=(T rhs) noexcept { m_Value |= static_cast<UnderlyingType>(rhs); return *this; }
		constexpr Flags& operator&=(T rhs) noexcept { m_Value &= static_cast<UnderlyingType>(rhs); return *this; }
		constexpr Flags& operator^=(T rhs) noexcept { m_Value ^= static_cast<UnderlyingType>(rhs); return *this; }
	private:
		UnderlyingType m_Value = 0;
	};
}