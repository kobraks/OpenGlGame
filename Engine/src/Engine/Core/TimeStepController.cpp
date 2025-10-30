#include "pch.h"
#include "TimeStepController.h"

namespace Engine {
	TimeStepController::TimeStepController(uint32_t fixedStepMs, uint64_t maxFixedSteps) : m_FixedStepIntervalMs(fixedStepMs), m_MaxFixedSteps(maxFixedSteps) {

	}

	TimeStepController::TimeStepController(double fixedStepHz, uint64_t maxFixedSteps) : m_MaxFixedSteps(maxFixedSteps) {
		fixedStepHz = std::clamp(fixedStepHz, 1.0, 1000.);
		m_FixedStepIntervalMs = static_cast<uint32_t>(std::lround(1000.0 / fixedStepHz));
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
		if (m_QueuedFixedSteps > 0) {
			if (m_FixedStepCount >= m_MaxFixedSteps) {
				return false;
			}

			--m_QueuedFixedSteps;
			m_NextFixedTimeMs += m_FixedStepIntervalMs;
			++m_FixedStepCount;
			return true;
		}

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

	void TimeStepController::StepFixedUpdates(uint32_t count) {
		if (!m_Paused || count == 0)
			return;

		const uint64_t maxAdd = std::numeric_limits<uint32_t>::max() - m_QueuedFixedSteps;
		m_QueuedFixedSteps += static_cast<uint32_t>(std::min<uint64_t>(count, maxAdd));
	}

	void TimeStepController::SetFixedStepHz(double hz) {
		hz = std::clamp(hz, 1.0, 1000.0);

		const uint32_t intervalMs = static_cast<uint32_t>(std::lround(1000.0 / hz));
		SetFixedStepIntervalMs(intervalMs);
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
		m_QueuedFixedSteps = 0;

		m_FrameDelta = Time::Zero;

		m_FrameClock.Restart();
		m_FixedClock.Restart();
		m_NextFixedTimeMs = m_FixedClock.GetElapsedTime().AsMilliseconds64();

		m_Paused = false;
		m_StepOneFrame = false;
	}
}
