#pragma once

#include "Engine/Core/Time.h"

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
