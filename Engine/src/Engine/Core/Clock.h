#pragma once
#include "Engine/Core/Base.h"

#include <chrono>
#include <ratio>
#include <type_traits>

namespace Engine {
	class Time;

	class Clock {
	public:
		Time GetElapsedTime() const;

		bool IsRunning() const;

		void Start();
		void Stop();

		Time Restart();
		Time Reset();
	private:
		using ClockImpl = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

		ClockImpl::time_point m_StartPoint{ClockImpl::now()};
		ClockImpl::time_point m_StopPoint;
	};
}