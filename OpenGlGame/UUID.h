#pragma once
#include <ostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <fmt/format.h>

namespace Game
{
	class UUID
	{
		boost::multiprecision::uint128_t m_UUID;

	public:
		UUID();
		explicit UUID(boost::multiprecision::uint128_t uuid);

		UUID(const UUID &) = default;

		operator boost::multiprecision::uint128_t() const { return m_UUID; }

		bool operator==(const UUID &right) const
		{
			return m_UUID == right.m_UUID;
		}

		std::strong_ordering operator<=>(const UUID &right) const
		{
			if(m_UUID == right.m_UUID)
				return std::strong_ordering::equal;
			if(m_UUID > right.m_UUID)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}

		friend std::ostream& operator<<(std::ostream &out, const UUID &right);

		template <typename T, typename Char, typename Enabled>
		friend struct fmt::formatter;
	};

	std::ostream& operator<<(std::ostream &out, const UUID &right);
}

template <>
struct fmt::formatter<Game::UUID>
{
	constexpr auto parse(format_parse_context &ctx)->decltype(ctx.begin())
	{
		return ctx.end();
	}

	template <typename FormatContext>
	auto format(const Game::UUID &input, FormatContext &ctx)->decltype(ctx.begin())
	{
		return format_to(
		                 ctx.out(),
		                 "{}",
		                 boost::multiprecision::to_string(static_cast<boost::multiprecision::uint128_t>(input))
		                );
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
