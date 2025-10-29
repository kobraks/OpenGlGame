#pragma once
#include "Engine/Core/Clock.h"
#include "Engine/Core/Time.h"

namespace Engine {
	class TimeStepController {
	public:
		TimeStepController(uint32_t fixedStepMs = 16, uint64_t maxFixedSteps = 60);

		void BeginFrame();
		void BeginFixedStepPhase();
		bool ShouldFixedUpdate();

		Time GetFrameDelta() const { return m_FrameDelta; }

		uint32_t GetFixedStepIntervalMs() const { return m_FixedStepIntervalMs; }
		void SetFixedStepIntervalMs(uint32_t ms);

		Time GetFixedDeltaTime() const { return Milliseconds(static_cast<int32_t>(m_FixedStepIntervalMs)); }
		Time GetScaledFixedDeltaTime() const { return GetFixedDeltaTime() * m_TimeScale; }

		uint64_t GetMaxFixedSteps() const { return m_MaxFixedSteps; }
		void SetMaxFixedSteps(uint64_t maxSteps) { m_MaxFixedSteps = maxSteps; }

		void SetTimeScale(float scale);
		float GetTimeScale() const { return m_TimeScale; }

		bool IsPaused() const { return m_Paused; }
		void Resume();
		void TogglePause() { m_Paused = !m_Paused; }
		void StepOneFrame() { if (m_Paused) m_StepOneFrame = true; }

		void Restart();

		Time GetCurrentFrameTime() const { return m_FrameClock.GetElapsedTime(); }
		Time GetCurrentFixedTime() const { return m_FixedClock.GetElapsedTime(); }

		uint64_t CurrentUpdateCount() const { return m_FixedStepCount; }
	private:
		Clock m_FrameClock;
		Clock m_FixedClock;

		uint32_t m_FixedStepIntervalMs = 16;
		uint64_t m_MaxFixedSteps = 60;

		uint64_t m_FixedStepCount = 0;

		uint64_t m_NextFixedTimeMs = 0;
		uint64_t m_CurrentFixedTimeMs = 0;

		float m_TimeScale = 1.0f;
		bool m_Paused = false;
		bool m_StepOneFrame = false;

		Time m_FrameDelta = Time::Zero;
	};
}
