#pragma once
#include "Engine/Core/Base.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/container_hash/hash.hpp>
#include <fmt/format.h>

namespace Engine {
	class UUID {
	public:
		using Type = boost::multiprecision::uint128_t;

		UUID();
		explicit UUID(Type id);

		operator Type() const { return m_ID; }

		constexpr bool operator==(const UUID &rth) const { return m_ID == rth.m_ID; }

		constexpr auto operator<=>(const UUID &rth) const {
			if (m_ID == rth.m_ID)
				return std::strong_ordering::equal;
			if (m_ID > rth.m_ID)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}

		friend std::istream &operator>>(std::istream &in, UUID &rth);
	private:
		Type m_ID;
	};
}

template<>
struct fmt::formatter<Engine::UUID> {
	uint8_t ShowBase : 1 = 0, Hex : 1 = 0, Upper : 1 = 0;

	auto constexpr parse(auto &ctx) {
		auto e = std::find(ctx.begin(), ctx.end(), '}');
		if (std::string_view f{ctx.begin(), e}; f == "#x")
			ShowBase = Hex = true;
		else if (f == "#X")
			ShowBase = Hex = Upper = true;
		else {
			Hex = (f == "x") || (f == "X");
			Upper = (f == "X");
		}

		return e;
	}

	auto format(const Engine::UUID &i, auto &ctx) const {
		auto f = Hex ? std::ios::hex : std::ios::dec;
		if (ShowBase)
			f |= std::ios::showbase;
		if (Upper)
			f |= std::ios::uppercase;

		auto s = static_cast<Engine::UUID::Type>(i).str(0, f);
		return std::copy(s.begin(), s.end(), ctx.out());
	}
};

template<>
struct std::hash<Engine::UUID> {
	size_t operator()(const Engine::UUID &i) const noexcept {
		return boost::multiprecision::hash_value(static_cast<Engine::UUID::Type>(i));
	}
};