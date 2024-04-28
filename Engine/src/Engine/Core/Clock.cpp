#include "pch.h"
#include "Engine/Core/Clock.h"
#include "Engine/Core/Time.h"

namespace Engine {
	Time Clock::GetElapsedTime() const {
		if(IsRunning())
			return std::chrono::duration_cast<std::chrono::nanoseconds>(ClockImpl::now() - m_StartPoint);
		return std::chrono::duration_cast<std::chrono::microseconds>(m_StopPoint - m_StartPoint);
	}

	bool Clock::IsRunning() const {
		return m_StopPoint == ClockImpl::time_point();
	}

	void Clock::Start() {
		if(!IsRunning()) {
			m_StartPoint += ClockImpl::now() - m_StopPoint;
			m_StopPoint = {};
		}
	}

	void Clock::Stop() {
		if(IsRunning())
			m_StopPoint = ClockImpl::now();
	}

	Time Clock::Restart() {
		const Time elapsed = GetElapsedTime();
		m_StartPoint       = ClockImpl::now();
		m_StopPoint        = {};

		return elapsed;
	}

	Time Clock::Reset() {
		const Time elapsed = GetElapsedTime();
		m_StartPoint       = ClockImpl::now();
		m_StopPoint        = m_StartPoint;

		return elapsed;
	}
}
