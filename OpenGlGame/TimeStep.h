#pragma once

namespace Game
{
	class TimeStep
	{
		float m_Time = 0.f;
	public:
		TimeStep(float time = 0.f) : m_Time(time) {}

		operator float() const {return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.f; }
	};
}
