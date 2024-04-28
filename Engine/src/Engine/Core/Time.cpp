#include "pch.h"
#include "Engine/Core/Time.h"

namespace Engine {
	const Time Time::Zero;

	constexpr Time::Time() = default;

	constexpr float Time::AsSeconds() const {
		return std::chrono::duration<float>(m_Microseconds).count();
	}

	constexpr int32_t Time::AsMilliseconds() const {
		return std::chrono::duration_cast<std::chrono::duration<int32_t, std::milli>>(m_Microseconds).count();
	}

	constexpr int64_t Time::AsMicroseconds() const {
		return m_Microseconds.count();
	}

	constexpr std::chrono::microseconds Time::ToDuration() const {
		return m_Microseconds;
	}

	constexpr Time Seconds(const float amount) {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(amount));
	}

	constexpr Time Milliseconds(const int32_t amount) {
		return std::chrono::milliseconds(amount);
	}

	constexpr Time Microseconds(const int64_t amount) {
		return std::chrono::microseconds(amount);
	}

	constexpr bool operator==(const Time &left, const Time &right) {
		return left.AsMicroseconds() == right.AsMicroseconds();
	}

	constexpr bool operator!=(const Time &left, const Time &right) {
		return left.AsMicroseconds() != right.AsMicroseconds();
	}

	constexpr bool operator<(const Time &left, const Time &right) {
		return left.AsMicroseconds() < right.AsMicroseconds();
	}

	constexpr bool operator>(const Time &left, const Time &right) {
		return left.AsMicroseconds() > right.AsMicroseconds();
	}

	constexpr bool operator<=(const Time &left, const Time &right) {
		return left.AsMicroseconds() <= right.AsMicroseconds();
	}

	constexpr bool operator>=(const Time &left, const Time &right) {
		return left.AsMicroseconds() >= right.AsMicroseconds();
	}

	constexpr Time operator-(const Time &right) {
		return Microseconds(-right.AsMicroseconds());
	}

	constexpr Time operator-(const Time &left, const Time &right) {
		return Microseconds(left.AsMicroseconds() - right.AsMicroseconds());
	}

	constexpr Time operator+(const Time &left, const Time &right) {
		return Microseconds(left.AsMicroseconds() + right.AsMicroseconds());
	}

	constexpr Time& operator-=(Time &left, const Time &right) {
		return left = left - right;
	}

	constexpr Time& operator+=(Time &left, const Time &right) {
		return left = left + right;
	}

	constexpr Time operator*(const Time &left, const float &right) {
		return Seconds(left.AsSeconds() * right);
	}

	constexpr Time operator*(const Time &left, const int64_t &right) {
		return Microseconds(left.AsMicroseconds() * right);
	}

	constexpr Time operator*(const float &left, const Time &right) {
		return right * left;
	}

	constexpr Time operator*(const int64_t &left, const Time &right) {
		return right * left;
	}

	constexpr Time& operator*=(Time &left, const int64_t &right) {
		return left = left * right;
	}

	constexpr Time& operator*=(Time &left, const float &right) {
		return left = left * right;
	}

	constexpr Time operator/(const Time &left, const float &right) {
		ENGINE_ASSERT(right != 0);
		return Seconds(left.AsSeconds() / right);
	}

	constexpr Time operator/(const Time &left, const int64_t &right) {
		ENGINE_ASSERT(right != 0);
		return Microseconds(left.AsMicroseconds() / right);
	}

	constexpr Time& operator/=(Time &left, const float &right) {
		ENGINE_ASSERT(right != 0);
		return left = left / right;
	}

	constexpr Time& operator/=(Time &left, const int64_t &right) {
		ENGINE_ASSERT(right != 0);
		return left = left / right;
	}

	constexpr float operator/(const Time &left, const Time &right) {
		ENGINE_ASSERT(right.AsMicroseconds() != 0);
		return left.AsSeconds() / right.AsSeconds();
	}

	constexpr Time operator%(const Time &left, const Time &right) {
		ENGINE_ASSERT(right.AsMicroseconds() != 0);
		return Microseconds(left.AsMicroseconds() % right.AsMicroseconds());
	}

	constexpr Time& operator%=(Time &left, const Time &right) {
		ENGINE_ASSERT(right.AsMicroseconds() != 0);
		return left = left % right;
	}
}