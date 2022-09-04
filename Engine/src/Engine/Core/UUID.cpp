#include "pch.h"
#include "Engine/Core/UUID.h"

#include <random>

namespace Game
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	static uint64_t Random()
	{
		return s_UniformDistribution(s_RandomDevice);
	}

	static constexpr auto Combine(const uint64_t &lth, const uint64_t &rth)
	{
		return (boost::multiprecision::uint128_t(lth) << 64) | rth;
	}

	UUID::UUID() : m_UUID(Combine(Random(), Random())) {}

	std::istream& operator>>(std::istream &in, UUID &right)
	{
		std::string number;
		in >> number;
		right.m_UUID = boost::multiprecision::uint128_t(number);

		return in;
	}
}
