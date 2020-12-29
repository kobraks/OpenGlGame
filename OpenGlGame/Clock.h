#pragma once

#include "Time.h"

namespace Game
{
	class Clock
	{
		Time m_StartTime;
	public:
		
		Clock();

		Time GetElapsedTime() const;
		Time Restart();
	};
}
