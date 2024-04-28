#pragma once
#include "Engine/Core/Base.h"

#include <cstdint>
#include <chrono>

namespace Engine {
	class Time {
	public:
		constexpr Time();

		template<typename Rep, typename Period>
		constexpr Time(const std::chrono::duration<Rep, Period> &duration);

		template<typename Rep, typename Period>
		constexpr operator std::chrono::duration<Rep, Period>() const;

		constexpr float AsSeconds() const;
		constexpr int32_t AsMilliseconds() const;
		constexpr int64_t AsMicroseconds() const;

		constexpr std::chrono::microseconds ToDuration() const;

		static const Time Zero;
	private:
		std::chrono::microseconds m_Microseconds{};

		explicit Time(uint64_t microseconds);

		friend constexpr Time Seconds(float);
		friend constexpr Time Milliseconds(uint32_t);
		friend constexpr Time Microseconds(uint64_t);
	};

	constexpr Time Seconds(float amount);
	constexpr Time Milliseconds(uint32_t amount);
	constexpr Time Microseconds(uint64_t amount);

	constexpr bool operator==(const Time &left, const Time &right);
	constexpr bool operator!=(const Time &left, const Time &right);
	constexpr bool operator<(const Time &left, const Time &right);
	constexpr bool operator>(const Time &left, const Time &right);
	constexpr bool operator<=(const Time &left, const Time &right);
	constexpr bool operator>=(const Time &left, const Time &right);

	constexpr Time operator-(const Time &right);
	constexpr Time operator-(const Time &left, const Time &right);
	constexpr Time operator+(const Time &left, const Time &right);

	constexpr Time& operator-=(Time &left, const Time &right);
	constexpr Time& operator+=(Time &left, const Time &right);

	constexpr Time operator*(const Time &left, const float &right);
	constexpr Time operator*(const Time &left, const int64_t &right);
	constexpr Time operator*(const float &left, const Time &right);
	constexpr Time operator*(const int64_t &left, const Time &right);

	constexpr Time& operator*=(Time &left, const int64_t &right);
	constexpr Time& operator*=(Time &left, const float &right);

	constexpr Time operator/(const Time &left, const float &right);
	constexpr Time operator/(const Time &left, const int64_t &right);

	constexpr Time& operator/=(Time &left, const float &right);
	constexpr Time& operator/=(Time &left, const int64_t &right);

	constexpr float operator/(const Time &left, const Time &right);

	constexpr Time operator%(const Time &left, const Time &right);
	constexpr Time& operator%=(Time &left, const Time &right);

	template <typename Rep, typename Period>
	constexpr Time::Time(const std::chrono::duration<Rep, Period> &duration) : m_Microseconds(duration) {}

	template <typename Rep, typename Period>
	constexpr Time::operator std::chrono::duration<Rep, Period>() const {
		return m_Microseconds;
	}
}
