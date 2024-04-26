#include "pch.h"
#include "Engine/Core/Clock.h"

#include <mutex>

#ifdef GAME_WINDOWS_PLATFORM
#include <Windows.h>

namespace {
	LARGE_INTEGER GetFrequency() {
		LARGE_INTEGER frequency;
		auto err = QueryPerformanceFrequency(&frequency);

		ENGINE_ASSERT(err == 0)
		if (err != 0) {
			
		}
		return frequency;
	}
}

#endif

namespace Engine {
	namespace Priv {
		class ClockImpl {
		public:
			static Time GetTime() {
#ifdef GAME_WINDOWS_PLATFORM
				static LARGE_INTEGER frequency = GetFrequency();

				LARGE_INTEGER time;
				QueryPerformanceFrequency(&time);

				return Microseconds(1000000 * time.QuadPart / frequency.QuadPart);
#else
				return Microseconds(0)
#endif
			}
		};
	}

	Clock::Clock() : m_StartTime(Priv::ClockImpl::GetTime()) {}

	Time Clock::GetElapsedTime() const {
		return Priv::ClockImpl::GetTime() - m_StartTime;
	}

	Time Clock::Restart() {
		const Time now = Priv::ClockImpl::GetTime();
		const Time elapsed = now - m_StartTime;

		m_StartTime = now;

		return elapsed;
	}
}