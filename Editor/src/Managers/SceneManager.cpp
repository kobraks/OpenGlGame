#include "SceneManager.h"

namespace Editor {
	void SceneManager::Play() {
		if (m_CurrentState == State::Simulate)
			Stop();

		m_CurrentState = State::Play;

		m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
	}

	void SceneManager::Simulate() {
		if (m_CurrentState == State::Play)
			Stop();

		m_CurrentState = State::Simulate;

		m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();
	}

	void SceneManager::Stop() {
		GAME_ASSERT(m_CurrentState == State::Play || m_CurrentState == State::Simulate);

		if (m_CurrentState == State::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_CurrentState == State::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_CurrentState = State::Edit;

		m_ActiveScene = m_EditorScene;
	}

	void SceneManager::Pause() {
		if (m_CurrentState == State::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

	void SceneManager::SetScene(const Engine::Ref<Engine::Scene>& scene) {
		m_EditorScene = m_ActiveScene = scene;
	}

	void SceneManager::New() {
		m_ActiveScene = m_EditorScene = Engine::Ref<Engine::Scene>();
	}
}
