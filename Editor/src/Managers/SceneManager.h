#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/EditorCamera.h"

namespace Editor {
	class SceneManager {
	public:
		enum class State {
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		SceneManager() = default;

		void Play();
		void Simulate();
		void Stop();
		void Pause();

		void SetScene(const Engine::Ref<Engine::Scene>& scene);

		Engine::Ref<Engine::Scene> GetActiveScene() const { return m_ActiveScene; }
		Engine::Ref<Engine::Scene> GetEditorScene() const { return m_EditorScene; }

		void New();

		State GetState() const { return m_CurrentState; }

	private:
		State m_CurrentState = State::Edit;
		Engine::Ref<Engine::Scene> m_ActiveScene;
		Engine::Ref<Engine::Scene> m_EditorScene;
	};
}