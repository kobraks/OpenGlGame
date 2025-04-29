#pragma once
#include "Engine/Core/Clock.h"
#include "Engine/Core/Time.h"

namespace Engine {
	class TimeStepController {
	public:
		TimeStepController(uint32_t updateRate = 16, uint64_t maxUpdates = 60);

		void BeginFrame();
		bool ShouldFixedUpdate();

		Time GetFrameDelta() const { return m_FrameDelta; }

		uint32_t GetUpdateRate() const { return m_UpdateRate; }
		void SetUpdateRate(uint32_t rateMs) { m_UpdateRate = rateMs; }

		uint64_t GetMaxUpdates() const { return m_MaxUpdates; }
		void SetMaxUpdates(uint64_t maxUpdates) { m_MaxUpdates = maxUpdates; }

		void SetTimeScale(float scale) { m_TimeScale = scale; }
		float GetTimeScale() const { return m_TimeScale; }

		bool IsPaused() const { return m_Paused; }
		void Resume() { m_Paused = false; }
		void TogglePause() { m_Paused = !m_Paused; }
		void StepOneFrame() { if (m_Paused) m_StepOneFrame = true; }

		void Restart();

		Time CurrentFrameTime() const { return m_FrameClock.GetElapsedTime(); }
		Time CurrentUpdateTime() const { return m_UpdateClock.GetElapsedTime(); }

		uint64_t CurrentUpdateCount() const { return m_UpdateCount; }
	private:
		Clock m_FrameClock;
		Clock m_UpdateClock;

		uint32_t m_UpdateRate = 16;
		uint64_t m_MaxUpdates = 60;

		uint64_t m_UpdateCount = 0;

		uint32_t m_NextUpdate = 0;

		float m_TimeScale = 1.0f;
		bool m_Paused = false;
		bool m_StepOneFrame = false;

		Time m_FrameDelta = Time::Zero;
	};
}
