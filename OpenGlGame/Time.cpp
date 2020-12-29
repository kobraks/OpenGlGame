#include "pch.h"
#include "Time.h"

namespace Game
{
	const Time Time::Zero;

	Time::Time(int64_t microseconds) : m_Microseconds(microseconds) {}
	Time::Time() : m_Microseconds(0) {}

	float Time::AsSeconds() const
	{
		return m_Microseconds / 1000000.f;
	}

	int32_t Time::AsMilliseconds() const
	{
		return static_cast<int32_t>(m_Microseconds / 1000);
	}

	int64_t Time::AsMicroseconds() const
	{
		return m_Microseconds;
	}

	Time Seconds(const float amount)
	{
		return Time(static_cast<int64_t>(amount * 1000000));
	}

	Time Milliseconds(const int32_t amount)
	{
		return Time(static_cast<int64_t>(amount) * 1000);
	}

	Time Microseconds(const int64_t amount)
	{
		return Time(amount);
	}

	bool operator==(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() == right.AsMicroseconds();
	}

	bool operator!=(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() != right.AsMicroseconds();
	}

	bool operator<(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() < right.AsMicroseconds();
	}

	bool operator>(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() > right.AsMicroseconds();
	}

	bool operator<=(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() <= right.AsMicroseconds();
	}

	bool operator>=(const Time &left, const Time &right)
	{
		return left.AsMicroseconds() >= right.AsMicroseconds();
	}

	Time operator-(const Time &right)
	{
		return Microseconds(-right.AsMicroseconds());
	}

	Time operator-(const Time &left, const Time &right)
	{
		return Microseconds(left.AsMicroseconds() - right.AsMicroseconds());
	}

	Time operator+(const Time &left, const Time &right)
	{
		return Microseconds(left.AsMicroseconds() + right.AsMicroseconds());
	}

	Time& operator-=(Time &left, const Time &right)
	{
		return left = left - right;
	}

	Time& operator+=(Time &left, const Time &right)
	{
		return left = left + right;
	}

	Time operator*(const Time &left, const float &right)
	{
		return Seconds(left.AsSeconds() * right);
	}
	
	Time operator*(const Time &left, const int64_t &right)
	{
		return Microseconds(left.AsMicroseconds() * right);
	}
	
	Time operator*(const float &left, const Time &right)
	{
		return right * left;
	}
	
	Time operator*(const int64_t &left, const Time &right)
	{
		return right * left;
	}
	
	Time& operator*=(Time &left, const int64_t &right)
	{
		return left = left * right;
	}
	
	Time& operator*=(Time &left, const float &right)
	{
		return left = left * right;
	}
	
	Time operator/(const Time &left, const float &right)
	{
		return Seconds(left.AsSeconds() / right);
	}
	
	Time operator/(const Time &left, const int64_t &right)
	{
		return Microseconds(left.AsMicroseconds() / right);
	}
	
	Time& operator/=(Time &left, const float &right)
	{
		return left = left / right;
	}
	
	Time& operator/=(Time &left, const int64_t &right)
	{
		return left = left / right;
	}
	
	float operator/(const Time &left, const Time &right)
	{
		return left.AsSeconds() / right.AsSeconds();
	}
	
	Time operator%(const Time &left, const Time &right)
	{
		return Microseconds(left.AsMicroseconds() % right.AsMicroseconds());
	}
	
	Time& operator%=(Time &left, const Time &right)
	{
		return left = left % right;
	}
}
