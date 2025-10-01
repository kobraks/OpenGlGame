#pragma once
#include "Engine/Core/Base.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/container_hash/hash.hpp>
#include <fmt/format.h>

namespace Engine {
	class UUID {
	public:
		using Type = boost::multiprecision::uint128_t;

		UUID() noexcept;
		explicit UUID(Type id) noexcept;

		explicit operator Type() const noexcept { return m_ID; }

		UUID& operator=(const UUID& rhs) noexcept = default;
		UUID& operator=(const Type& rhs) noexcept { m_ID = rhs; return *this; }

		[[nodiscard]] constexpr bool operator==(Type rhs) const noexcept { return m_ID == rhs; }

		[[nodiscard]] constexpr auto operator<=>(Type rhs) const noexcept {
			if (m_ID == rhs)
				return std::strong_ordering::equal;
			if (m_ID > rhs)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}

		[[nodiscard]] constexpr bool operator==(const UUID &rhs) const { return m_ID == rhs.m_ID; }

		[[nodiscard]] constexpr auto operator<=>(const UUID &rhs) const {
			if (m_ID == rhs.m_ID)
				return std::strong_ordering::equal;
			if (m_ID > rhs.m_ID)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;

		}

		[[nodiscard]] static UUID FromString(const std::string& name);

		friend std::istream &operator>>(std::istream &in, UUID &rhs);

		static UUID Invalid() noexcept { return UUID(0); }
		[[nodiscard]] constexpr bool IsValid() const noexcept { return m_ID != 0; }
	private:
		Type m_ID;
	};
}

template<>
struct fmt::formatter<Engine::UUID> {
	bool ShowBase = false;
	bool Hex = false;
	bool Upper = false;

	auto constexpr parse(auto& ctx) {
		auto it = ctx.begin(), end = ctx.end();
		std::string_view spec{ it, static_cast<size_t>(std::distance(it, end)) };

		if (spec == "#x") { ShowBase = true; Hex = true; }
		else if (spec == "#X") { ShowBase = true; Hex = true; Upper = true; }
		else if (spec == "x") { Hex = true; }
		else if (spec == "X") { Hex = true; Upper = true; }
		else if (!spec.empty())
			throw fmt::format_error("invalid format specifier");

		return end;
	}

	auto format(const Engine::UUID& i, auto& ctx) const {
		std::ios_base::fmtflags flags = Hex ? std::ios::hex : std::ios::dec;
		if (ShowBase)
			flags |= std::ios::showbase;
		if (Upper)
			flags |= std::ios::uppercase;

		const auto s = static_cast<Engine::UUID::Type>(i).str(0, flags);
		return std::copy(s.begin(), s.end(), ctx.out());
	}
};

template<>
struct std::hash<Engine::UUID> {
	std::size_t operator()(const Engine::UUID& i) const noexcept {
		return boost::multiprecision::hash_value(static_cast<Engine::UUID::Type>(i));
	}
};