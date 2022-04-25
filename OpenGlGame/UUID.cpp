#include "pch.h"
#include "UUID.h"

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

	constexpr boost::multiprecision::uint128_t Combine(const uint64_t &left, const uint64_t &right)
	{
		return boost::multiprecision::uint128_t(left) << 64 | right;
	}

	UUID::UUID() : m_UUID(Combine(Random(), Random())) {}

	UUID::UUID(boost::multiprecision::uint128_t uuid) : m_UUID(uuid) {}
}
