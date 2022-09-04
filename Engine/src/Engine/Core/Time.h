#pragma once

#include "Engine/Core/Base.h"

namespace Game
{
	class Time
	{
		int64_t m_Microseconds = 0;

		explicit Time(int64_t microseconds);

		friend Time Seconds(const float);
		friend Time Milliseconds(const int32_t);
		friend Time Microseconds(const int64_t);
	public:
		Time();

		float AsSeconds() const;
		int32_t AsMilliseconds() const;
		int64_t AsMicroseconds() const;

		static const Time Zero;
	};

	Time Seconds(const float amount);
	Time Milliseconds(const int32_t amount);
	Time Microseconds(const int64_t amount);

	bool operator==(const Time &left, const Time &right);
	bool operator!=(const Time &left, const Time &right);
	bool operator<(const Time &left, const Time &right);
	bool operator>(const Time &left, const Time &right);
	bool operator<=(const Time &left, const Time &right);
	bool operator>=(const Time &left, const Time &right);

	Time operator-(const Time& right);
	Time operator-(const Time& left, const Time& right);
	Time operator+(const Time& left, const Time& right);
	
	Time& operator-=(Time& left, const Time& right);
	Time& operator+=(Time& left, const Time& right);
	
	Time operator*(const Time& left, const float& right);
	Time operator*(const Time& left, const int64_t& right);
	Time operator*(const float& left, const Time& right);
	Time operator*(const int64_t& left, const Time& right);
	
	Time&operator*=(Time& left, const int64_t& right);
	Time& operator*=(Time& left, const float& right);

	Time operator/(const Time& left, const float& right);
	Time operator/(const Time& left, const int64_t& right);
	
	Time& operator/=(Time& left, const float& right);
	Time& operator/=(Time& left, const int64_t& right);

	float operator/(const Time& left, const Time& right);
	
	Time operator%(const Time& left, const Time& right);
	Time& operator%=(Time& left, const Time& right);

	namespace Literals
	{
		inline Time operator"" _Sec(long double time)
		{
			return Seconds(static_cast<float>(time));
		}

		inline Time operator"" _Ms(unsigned long long time)
		{
			return Milliseconds(static_cast<int32_t>(time));
		}

		inline Time operator"" _Ns(unsigned long long time)
		{
			return Microseconds(static_cast<int64_t>(time));
		}
	}
}
