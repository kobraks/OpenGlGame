#include "pch.h"
#include "TimeStepController.h"

namespace Engine {
	TimeStepController::TimeStepController(uint32_t fixedStepMs, uint64_t maxFixedSteps) : m_FixedStepIntervalMs(fixedStepMs), m_MaxFixedSteps(maxFixedSteps) {

	}

	void TimeStepController::BeginFrame() {
		if (m_Paused && m_StepOneFrame) {
			m_Paused = false;
		}

		if (m_Paused && !m_StepOneFrame) {
			m_FrameClock.Restart();
			m_FrameDelta = Time::Zero;
		}
		else {
			m_FrameDelta = m_FrameClock.Restart() * m_TimeScale;
		}
	}

	void TimeStepController::BeginFixedStepPhase() {
		m_CurrentFixedTimeMs = m_FixedClock.GetElapsedTime().AsMilliseconds64();

		if ((m_CurrentFixedTimeMs - m_NextFixedTimeMs) > (m_MaxFixedSteps * m_FixedStepIntervalMs)) {
			m_NextFixedTimeMs = m_CurrentFixedTimeMs;
		}

		m_FixedStepCount = 0;
	}

	bool TimeStepController::ShouldFixedUpdate() {
		if (m_Paused && !m_StepOneFrame)
			return false;

		const uint64_t now = m_FixedClock.GetElapsedTime().AsMilliseconds64();

		// If we're not yet due for the next update, return false
		if ((now - m_NextFixedTimeMs) < m_FixedStepIntervalMs) {
			if (m_StepOneFrame && !m_Paused) {
				m_Paused = true;
				m_StepOneFrame = false;
			}
			return false;
		}

		// Clamp to max updates per frame
		if (m_FixedStepCount >= m_MaxFixedSteps) {
			if (m_StepOneFrame && !m_Paused) {
				m_Paused = true;
				m_StepOneFrame = false;
			}
			return false;
		}

		m_NextFixedTimeMs += m_FixedStepIntervalMs;
		++m_FixedStepCount;

		return true;
	}

	void TimeStepController::SetFixedStepIntervalMs(uint32_t ms) {
		m_FixedStepIntervalMs = ms;
		m_NextFixedTimeMs = m_FixedClock.GetElapsedTime().AsMilliseconds64();
	}

	void TimeStepController::SetTimeScale(float scale) {
		m_TimeScale = scale;

		if (scale == 0.0f)
			m_Paused = true;
		else if (m_Paused)
			m_Paused = false;
	}

	void TimeStepController::Resume() {
		if (m_TimeScale != 0.0f) {
			m_Paused = false;
			m_FrameClock.Restart();
			m_FixedClock.Restart();
		}
	}

	void TimeStepController::Restart() {
		m_FixedStepCount = 0;
		m_FrameDelta = Time::Zero;

		m_FrameClock.Restart();
		m_FixedClock.Restart();
		m_NextFixedTimeMs = m_FixedClock.GetElapsedTime().AsMilliseconds64();

		m_Paused = false;
		m_StepOneFrame = false;
	}
}
