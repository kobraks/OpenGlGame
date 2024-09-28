#pragma once
#include "Engine/Core/Base.h"

#include <cstdint>
#include <chrono>

namespace Engine {
	class Time {
	public:
		constexpr Time() = default;

		template<typename Rep, typename Period>
		constexpr Time(const std::chrono::duration<Rep,Period> &duration) : m_Microseconds(duration) {}

		template<typename Rep, typename Period>
		constexpr operator std::chrono::duration<Rep, Period>() const { return m_Microseconds; };

		constexpr float AsSeconds() const { return std::chrono::duration<float>(m_Microseconds).count(); }
		constexpr int32_t AsMilliseconds() const { return std::chrono::duration_cast<std::chrono::duration<int32_t, std::milli>>(m_Microseconds).count(); }
		constexpr int64_t AsMicroseconds() const { return m_Microseconds.count(); }

		constexpr std::chrono::microseconds ToDuration() const { return m_Microseconds; }

		static const Time Zero;

	private:
		std::chrono::microseconds m_Microseconds{};

		friend constexpr Time Seconds(float);
		friend constexpr Time Milliseconds(int32_t);
		friend constexpr Time Microseconds(int64_t);
	};

	constexpr Time Seconds(float seconds) {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(seconds));
	}

	constexpr Time Milliseconds(int32_t milliseconds) {
		return std::chrono::milliseconds(milliseconds);
	}

	constexpr Time Microseconds(int64_t microseconds) {
		return std::chrono::microseconds(microseconds);
	}

	constexpr bool operator==(Time lth, Time rth) {
		return lth.AsMicroseconds() == rth.AsMicroseconds();
	}

	constexpr bool operator!=(Time lth, Time rth) {
		return lth.AsMicroseconds() != rth.AsMicroseconds();
	}

	constexpr bool operator< (Time lth, Time rth) {
		return lth.AsMicroseconds() < rth.AsMicroseconds();
	}

	constexpr bool operator> (Time lth, Time rth) {
		return lth.AsMicroseconds() > rth.AsMicroseconds();
	}

	constexpr bool operator<= (Time lth, Time rth) {
		return lth.AsMicroseconds() <= rth.AsMicroseconds();
	}

	constexpr bool operator>= (Time lth, Time rth) {
		return lth.AsMicroseconds() >= rth.AsMicroseconds();
	}

	constexpr Time operator-(Time rth) {
		return Microseconds(-rth.AsMicroseconds());
	}

	constexpr Time operator+ (Time lth, Time rth) {
		return Microseconds(lth.AsMicroseconds() + rth.AsMicroseconds());
	}

	constexpr Time &operator+= (Time &lth, Time rth) {
		return lth = lth + rth;
	}

	constexpr Time operator-(Time lth, Time rth) {
		return Microseconds(lth.AsMicroseconds() - rth.AsMicroseconds());
	}

	constexpr Time &operator-=(Time &lth, Time rth) {
		return lth = lth - rth;
	}

	constexpr Time operator*(Time lth, float rth) {
		return Seconds(lth.AsSeconds() * rth);
	}

	constexpr Time operator*(Time lth, int64_t rth) {
		return Microseconds(lth.AsMicroseconds() * rth);
	}

	constexpr Time operator*(float lth, Time rth) {
		return rth * lth;
	}

	constexpr Time operator*(int64_t lth, Time rth) {
		return rth * lth;
	}

	constexpr Time &operator*=(Time &lth, float rth) {
		return lth = lth * rth;
	}

	constexpr Time &operator*=(Time &lth, int64_t rth) {
		return lth = lth * rth;
	}

	constexpr Time operator/(Time lth, float rth) {
		ENGINE_ASSERT(rth != 0);

		return Seconds(lth.AsSeconds() / rth);
	}

	constexpr Time operator/(Time lth, int64_t rth) {
		ENGINE_ASSERT(rth != 0);

		return Microseconds(lth.AsMicroseconds() / rth);
	}

	constexpr Time &operator/=(Time &lth, float rth) {
		ENGINE_ASSERT(rth != 0);

		return lth = lth / rth;
	}

	constexpr Time &operator/=(Time &lth, int64_t rth) {
		ENGINE_ASSERT(rth != 0);

		return lth = lth / rth;
	}

	constexpr float operator/(Time lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return lth.AsSeconds() / rth.AsSeconds();
	}

	constexpr Time operator%(Time lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return Microseconds(lth.AsMicroseconds() % rth.AsMicroseconds());
	}

	constexpr Time &operator%=(Time &lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return lth = lth % rth;
	}

	inline constexpr Time Time::Zero;
}
