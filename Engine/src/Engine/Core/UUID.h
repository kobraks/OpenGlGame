#pragma once

#include "Engine/Core/Base.h"

#include <istream>
#include <ostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <fmt/format.h>


namespace Game
{
	class UUID
	{
	public:
		using NumericType = boost::multiprecision::uint128_t;

	private:
		NumericType m_UUID;

	public:
		UUID();
		explicit UUID(NumericType uuid);

		operator NumericType() const { return m_UUID; }

		constexpr bool operator==(const UUID &right) const { return m_UUID == right.m_UUID; }

		constexpr std::strong_ordering operator<=>(const UUID &right) const
		{
			if(m_UUID == right.m_UUID)
				return std::strong_ordering::equal;
			if(m_UUID > right.m_UUID)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}

		template <typename OStream>
		friend OStream& operator<<(OStream &out, const UUID &right);
		friend std::istream& operator>>(std::istream &in, UUID &right);

		template <typename T, typename Char, typename Enabled>
		friend struct fmt::formatter;
	};

	template <typename OStream>
	OStream& operator<<(OStream &out, const UUID &right)
	{
		return out << right.m_UUID.str();
	}
}

template <>
struct fmt::formatter<Game::UUID>: formatter<std::size_t>
{
	template <typename FormatContext>
	auto format(const Game::UUID &input, FormatContext &ctx)->decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{}", input.m_UUID.str());
	}
};

namespace std
{
	template <>
	struct hash<Game::UUID>
	{
		size_t operator()(const Game::UUID &uuid) const noexcept
		{
			return boost::multiprecision::hash_value(static_cast<boost::multiprecision::uint128_t>(uuid));
		}
	};
}
