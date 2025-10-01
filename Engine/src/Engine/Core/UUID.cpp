#include "pch.h"
#include "Engine/Core/UUID.h"

#include <random>

namespace Engine {
	thread_local std::mt19937_64 th_Engine{ std::random_device{}() };

	static uint64_t Random64() noexcept {
		std::uniform_int_distribution<uint64_t> dist;
		return dist(th_Engine);
	}

	inline UUID::Type Combine(uint64_t hi, uint64_t lo) noexcept {
		return (static_cast<UUID::Type>(hi) << 64) | static_cast<UUID::Type>(lo);
	}


	UUID::UUID() noexcept : m_ID(Combine(Random64(), Random64())){}

	UUID::UUID(Type id) noexcept : m_ID(id) {}

	UUID UUID::FromString(const std::string& name) {
		static constexpr char salt[] = "ENGINE_ASSET_SALT";
		boost::hash<std::string> hasher;

		uint64_t hi = hasher(name);
		uint64_t lo = hasher(name + salt);
		return UUID(Combine(hi, lo));

	}

	std::istream &operator>>(std::istream &in, UUID &rhs) {
		boost::multiprecision::cpp_int temp;
		in >> temp;
		rhs.m_ID = static_cast<UUID::Type>(temp);

		return in;
	}
}
