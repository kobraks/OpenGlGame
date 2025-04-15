#include "pch.h"
#include "Engine/Core/UUID.h"

#include <random>

namespace Engine {
	static std::mt19937_64 s_Engine(std::random_device{}());
	static std::uniform_int_distribution<uint64_t> s_Uniform;

	static uint64_t Random() noexcept {
		return s_Uniform(s_Engine);
	}

	template<typename R, typename Arg>
	static constexpr R Combine(const Arg lth) noexcept {
		return lth;
	}

	template <typename R, typename Arg, typename ...Args>
	static constexpr R Combine(const Arg lth, const Args &&... args) noexcept {
		return (R(lth) << 64) + Combine<R, Args...>(args...);
	}

	template<typename T>
	static T GetRandom();

	template<>
	static UUID::Type GetRandom() {
		return Combine<UUID::Type>(Random(), Random());
	}

	UUID::UUID() : m_ID(GetRandom<UUID::Type>()){}

	UUID::UUID(Type id) : m_ID(id) {
	}

	std::istream &operator>>(std::istream &in, UUID &rth) {
		std::string number;
		in >> number;
		rth.m_ID = UUID::Type(number);

		return in;
	}
}
