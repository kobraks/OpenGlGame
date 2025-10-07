#pragma once
#include "Engine/Core/Base.h"

#include <type_traits>

namespace Engine {
	template <typename Enum>
	class Flags {
		static_assert(std::is_enum_v<Enum>, "Template parameter 'Enum' must be an enum type.");

	public:
		using UnderlyingType = std::underlying_type_t<Enum>;

		constexpr Flags() = default;
		constexpr explicit Flags(Enum e) : m_Value(static_cast<UnderlyingType>(e)) {}
		constexpr explicit Flags(UnderlyingType e) : m_Value(e) {}

		constexpr bool operator==(Enum e) const noexcept { return m_Value == static_cast<UnderlyingType>(e); }
		constexpr bool operator!=(Enum e) const noexcept { return !(*this == e); }

		[[nodiscard]] constexpr bool HasAll(Enum e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) == static_cast<UnderlyingType>(e); }
		[[nodiscard]] constexpr bool HasAny(Enum e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) != 0; }
		[[nodiscard]] constexpr bool HasNone(Enum e) const noexcept { return (m_Value & static_cast<UnderlyingType>(e)) == 0; }
		[[nodiscard]] constexpr bool IsNone() const noexcept { return m_Value == 0; }

		constexpr void Disable(Enum e) noexcept { m_Value &= ~static_cast<UnderlyingType>(e); }
		constexpr void Enable(Enum e) noexcept { m_Value |= static_cast<UnderlyingType>(e); }
		constexpr void Toggle(Enum e) noexcept { m_Value ^= static_cast<UnderlyingType>(e); }
		constexpr void Set(Enum e, bool enabled) noexcept {
			if (enabled) m_Value |= static_cast<UnderlyingType>(e);
			else m_Value &= ~static_cast<UnderlyingType>(e);
		}
		constexpr void Clear() noexcept { m_Value = 0; }

		[[nodiscard]] constexpr Enum Get() const noexcept { return static_cast<Enum>(m_Value); }
		[[nodiscard]] constexpr UnderlyingType Raw() const noexcept { return m_Value; }

		friend constexpr Flags operator | (Flags a, Flags b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value | b.m_Value) }; }
		friend constexpr Flags operator & (Flags a, Flags b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value & b.m_Value) }; }

		friend constexpr Flags operator | (Flags a, Enum b) noexcept { a.Enable(b); return a; }
		friend constexpr Flags operator & (Flags a, Enum b) noexcept { return Flags{ static_cast<UnderlyingType>(a.m_Value & static_cast<UnderlyingType>(b)) }; }
		friend constexpr Flags operator | (Enum a, Flags b) noexcept { return b | a; }
		friend constexpr Flags operator & (Enum a, Flags b) noexcept { return b & a; }

		// friend constexpr Flags operator |(Enum a, Enum b) noexcept { return Flags{ static_cast<UnderlyingType>(static_cast<UnderlyingType>(a) | static_cast<UnderlyingType>(b)) }; }
		// friend constexpr Flags operator &(Enum a, Enum b) noexcept { return Flags{ static_cast<UnderlyingType>(static_cast<UnderlyingType>(a) & static_cast<UnderlyingType>(b)) }; }

		constexpr Flags& operator|=(Flags rhs) noexcept { m_Value |= rhs.m_Value; return *this; }
		constexpr Flags& operator&=(Flags rhs) noexcept { m_Value &= rhs.m_Value; return *this; }
		constexpr Flags& operator|=(Enum rhs) noexcept { Enable(rhs); return *this; }
		constexpr Flags& operator&=(Enum rhs) noexcept { m_Value &= static_cast<UnderlyingType>(rhs); return *this; }
	private:
		UnderlyingType m_Value = 0;
	};
}