#include "SceneContext.h"

namespace Editor {
	void SceneContext::New() {
		m_EditorScene = Engine::MakeRef<Engine::Scene>();
		m_ActiveScene = m_EditorScene;

		LOG_INFO("New Scene created");
	}

	void SceneContext::Set(const Engine::Ref<Engine::Scene>& scene) {
		m_EditorScene = m_ActiveScene = scene;
	}

	void SceneContext::CopyToActiveScene(bool runtime) {
		m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
		if (runtime) {
			m_ActiveScene->OnRuntimeStart();
		} else {
			m_ActiveScene->OnSimulationStart();
		}
	}

	void SceneContext::ResetToEditorScene() {
		m_ActiveScene = m_EditorScene;
	}

	bool SceneContext::IsPaused() const {
		return m_ActiveScene && m_ActiveScene->IsPaused();
	}

	void SceneContext::SetPaused(bool paused) {
		if (m_ActiveScene) {
			m_ActiveScene->SetPaused(paused);
		} else {
			LOG_WARN("No active scene to set paused state.");
		}
	}

	void SceneContext::Step() {
		if (m_ActiveScene) {
			m_ActiveScene->Step();
		} else {
			LOG_WARN("No active scene to step through.");
		}
	}
}
