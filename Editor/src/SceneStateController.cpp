#include "SceneStateController.h"

namespace Editor {
	SceneStateController::SceneStateController(SceneContext* context) : m_Context(context) {
	}

	void SceneStateController::Play() {
		GAME_ASSERT(m_Context);

		if (m_State == State::Simulate)
			Stop();

		m_State = State::Play;
		m_Context->CopyToActiveScene(true);
	}

	void SceneStateController::Simulate() {
		GAME_ASSERT(m_Context);

		if (m_State == State::Play)
			Stop();

		m_State = State::Simulate;

		m_Context->CopyToActiveScene(false);
	}

	void SceneStateController::Stop() {
		GAME_ASSERT(m_Context);
		GAME_ASSERT(m_State == State::Play || m_State == State::Simulate);

		if (m_State == State::Play)
			m_Context->GetActiveScene()->OnRuntimeStop();
		else if (m_State == State::Simulate)
			m_Context->GetActiveScene()->OnSimulationStop();

		m_Context->ResetToEditorScene();
		m_State = State::Edit;
	}

	void SceneStateController::Pause(){
		GAME_ASSERT(m_Context);

		if (m_State == State::Edit)
			return;

		m_Context->SetPaused(true);
	}
}
