#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Time.h"

namespace Engine {
	class Clock {
	public:
		Clock();

		Time GetElapsedTime() const;
		Time Restart();
	private:
		Time m_StartTime;
	};
}