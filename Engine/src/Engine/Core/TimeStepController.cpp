#include "pch.h"
#include "TimeStepController.h"

namespace Engine {
	TimeStepController::TimeStepController(uint32_t updateRate, uint64_t maxUpdates) : m_UpdateRate(updateRate), m_MaxUpdates(maxUpdates) {

	}

	void TimeStepController::BeginFrame() {
		if (m_Paused && !m_StepOneFrame)
			m_FrameDelta = Time::Zero;
		else {
			m_FrameDelta = m_FrameClock.Restart() * m_TimeScale;
		}

		if (m_StepOneFrame) {
			m_Paused = true;
			m_StepOneFrame = false;
		}

		m_UpdateCount = 0;
	}

	bool TimeStepController::ShouldFixedUpdate() {
		if (m_Paused && !m_StepOneFrame)
			return false;

		const uint32_t now = m_UpdateClock.GetElapsedTime().AsMilliseconds();
		if ((now - m_NextUpdate) >= m_UpdateRate) {
			if (m_UpdateCount <= m_NextUpdate) {
				m_NextUpdate += m_UpdateRate;
				++m_UpdateCount;

				return true;
			} else {
				m_NextUpdate = now;
			}
		}

		return false;
	}

	void TimeStepController::SetTimeScale(float scale) {
		m_TimeScale = scale;

		if (scale == 0.0f)
			m_Paused = true;
		else if (m_Paused)
			m_Paused = false;
	}

	void TimeStepController::Resume() {
		if (m_TimeScale != 0.0f)
			m_Paused = false;
	}

	void TimeStepController::Restart() {
		m_UpdateCount = 0;
		m_NextUpdate = 0;
		m_FrameDelta = Time::Zero;

		m_FrameClock.Restart();
		m_UpdateClock.Restart();

		m_Paused = false;
		m_StepOneFrame = false;
	}
}
