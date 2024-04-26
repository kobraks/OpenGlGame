#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	class Time {
	public:
		Time();

		float AsSeconds() const;
		uint32_t AsMilliseconds() const;
		uint64_t AsMicroseconds() const;

		static const Time Zero;
	private:
		uint64_t m_Microseconds = 0;

		explicit Time(uint64_t microseconds);

		friend Time Seconds(float);
		friend Time Milliseconds(uint32_t);
		friend Time Microseconds(uint64_t);
	};

	Time Seconds(float amount);
	Time Milliseconds(uint32_t amount);
	Time Microseconds(uint64_t amount);

	bool operator==(const Time &left, const Time &right);
	bool operator!=(const Time &left, const Time &right);
	bool operator<(const Time &left, const Time &right);
	bool operator>(const Time &left, const Time &right);
	bool operator<=(const Time &left, const Time &right);
	bool operator>=(const Time &left, const Time &right);

	Time operator-(const Time &right);
	Time operator-(const Time &left, const Time &right);
	Time operator+(const Time &left, const Time &right);

	Time& operator-=(Time &left, const Time &right);
	Time& operator+=(Time &left, const Time &right);

	Time operator*(const Time &left, const float &right);
	Time operator*(const Time &left, const int64_t &right);
	Time operator*(const float &left, const Time &right);
	Time operator*(const int64_t &left, const Time &right);

	Time& operator*=(Time &left, const int64_t &right);
	Time& operator*=(Time &left, const float &right);

	Time operator/(const Time &left, const float &right);
	Time operator/(const Time &left, const int64_t &right);

	Time& operator/=(Time &left, const float &right);
	Time& operator/=(Time &left, const int64_t &right);

	float operator/(const Time &left, const Time &right);

	Time operator%(const Time &left, const Time &right);
	Time& operator%=(Time &left, const Time &right);
}