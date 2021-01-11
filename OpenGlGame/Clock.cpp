#include "pch.h"
#include "Clock.h"

#include <windows.h>
#include <mutex>

namespace
{
	LARGE_INTEGER GetFrequency()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency;
    }
}

namespace Game
{
	namespace Priv
	{
		class ClockImpl
		{
		public:
			static Time GetTime()
			{
				static LARGE_INTEGER frequency = GetFrequency();

				LARGE_INTEGER time;
				QueryPerformanceCounter(&time);

				return Microseconds(1000000 * time.QuadPart / frequency.QuadPart);
			}
		};
	}
	
	Clock::Clock() : m_StartTime(Priv::ClockImpl::GetTime()) {}
	Time Clock::GetElapsedTime() const
	{
		return Priv::ClockImpl::GetTime() - m_StartTime;
	}
	
	Time Clock::Restart()
	{
		Time now = Priv::ClockImpl::GetTime() ;
		const Time elapesed = now - m_StartTime;
		m_StartTime = now;

		return elapesed;
	}
}
