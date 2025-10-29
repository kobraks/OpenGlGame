#pragma once
#include "Engine/Core/Base.h"

#include <cstdint>
#include <chrono>

#include <fmt/format.h>

namespace Engine {
	class Time {
	public:
		constexpr Time() = default;

		template<typename Rep, typename Period>
		constexpr Time(const std::chrono::duration<Rep,Period> &duration) : m_Microseconds(duration) {}

		template<typename Rep, typename Period>
		constexpr operator std::chrono::duration<Rep, Period>() const { return m_Microseconds; };

		template<class ToDuration = std::chrono::microseconds>
		constexpr ToDuration ToChrono() const {
			return std::chrono::duration_cast<ToDuration>(m_Microseconds);
		}

		constexpr float AsSeconds() const { return std::chrono::duration<float>(m_Microseconds).count(); }
		constexpr double AsSecondsD() const { return std::chrono::duration<double>(m_Microseconds).count(); }

		constexpr int32_t AsMilliseconds() const { return std::chrono::duration_cast<std::chrono::duration<int32_t, std::milli>>(m_Microseconds).count(); }
		constexpr int64_t AsMilliseconds64() const { return std::chrono::duration_cast<std::chrono::milliseconds>(m_Microseconds).count(); }

		constexpr int64_t AsMicroseconds() const { return m_Microseconds.count(); }

		constexpr std::chrono::microseconds ToDuration() const { return m_Microseconds; }

		static const Time Zero;

		template<class Rep, typename Period>
		static constexpr Time FromChrono(std::chrono::duration<Rep, Period> duration) {
			return Time{ std::chrono::duration_cast<std::chrono::microseconds>(duration) };
		}

	private:
		std::chrono::microseconds m_Microseconds{};

		friend constexpr Time Seconds(float);
		friend constexpr Time Seconds(double);
		friend constexpr Time Milliseconds(int32_t);
		friend constexpr Time Microseconds(int64_t);
	};

	constexpr Time Seconds(float seconds) {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(seconds));
	}

	constexpr Time Seconds(double seconds) {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(seconds));
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

	constexpr Time operator*(Time lth, double rth) {
		return Seconds(lth.AsSecondsD() * rth);
	}

	constexpr Time operator*(Time lth, int64_t rth) {
		return Microseconds(lth.AsMicroseconds() * rth);
	}

	constexpr Time operator*(float lth, Time rth) {
		return rth * lth;
	}

	constexpr Time operator*(double lth, Time rth) {
		return rth * lth;
	}

	constexpr Time operator*(int64_t lth, Time rth) {
		return rth * lth;
	}

	constexpr Time &operator*=(Time &lth, float rth) {
		return lth = lth * rth;
	}

	constexpr Time& operator*=(Time& lth, double rth) {
		return lth = lth * rth;
	}

	constexpr Time &operator*=(Time &lth, int64_t rth) {
		return lth = lth * rth;
	}

	constexpr Time operator/(Time lth, float rth) {
		ENGINE_ASSERT(rth != 0);

		return Seconds(lth.AsSeconds() / rth);
	}

	constexpr Time operator/(Time lth, double rth) {
		ENGINE_ASSERT(rth != 0);

		return Seconds(lth.AsSecondsD() / rth);
	}

	constexpr Time operator/(Time lth, int64_t rth) {
		ENGINE_ASSERT(rth != 0);

		return Microseconds(lth.AsMicroseconds() / rth);
	}

	constexpr Time &operator/=(Time &lth, float rth) {
		ENGINE_ASSERT(rth != 0);

		return lth = lth / rth;
	}

	constexpr Time& operator/=(Time& lth, double rth) {
		ENGINE_ASSERT(rth != 0);

		return lth = lth / rth;
	}

	constexpr Time &operator/=(Time &lth, int64_t rth) {
		ENGINE_ASSERT(rth != 0);

		return lth = lth / rth;
	}

	constexpr double operator/(Time lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return lth.AsSecondsD() / rth.AsSecondsD();
	}

	constexpr Time operator%(Time lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return Microseconds(lth.AsMicroseconds() % rth.AsMicroseconds());
	}

	constexpr Time &operator%=(Time &lth, Time rth) {
		ENGINE_ASSERT(rth.AsMicroseconds() != 0);

		return lth = lth % rth;
	}

	inline const Time Time::Zero;
}

template<>
struct fmt::formatter<Engine::Time> : fmt::formatter<int64_t> {
	constexpr auto format(const Engine::Time &time, format_context &ctx) {
		if (m_Milliseconds)
			return fmt::format_to(ctx.out(), "{}ms", time.AsMilliseconds64());
		if (m_Seconds)
			return fmt::format_to(ctx.out(), "{:#.4f}s", time.AsSecondsD());

		return fmt::format_to(ctx.out(), "{}us", time.AsMicroseconds());
	}

	constexpr auto parse(format_parse_context &ctx) {
		const auto end = std::ranges::find(ctx, '}');
		if (end != ctx.begin()) {
			const char rep = *ctx.begin();

			if (rep == 'm')
				m_Milliseconds = true;
			if (rep == 's')
				m_Seconds = true;

			ctx.advance_to(std::next(ctx.begin()));
		}

		return ctx.begin();
	}

private:
	bool m_Milliseconds = false;
	bool m_Seconds = false;
};

namespace std {
	template <>
	struct hash<Engine::Time> {
		std::size_t operator()(const Engine::Time& time) const noexcept {
			return std::hash<int64_t>{}(time.AsMicroseconds());
		}
	};
}