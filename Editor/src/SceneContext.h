#pragma once

#include "Engine/Scene/Scene.h"

namespace Editor {
	class SceneContext {
	public:
		void New();
		void Set(const Engine::Ref<Engine::Scene>& scene);

		Engine::Ref<Engine::Scene> GetActiveScene() const { return m_ActiveScene; }
		Engine::Ref<Engine::Scene> GetEditorScene() const { return m_EditorScene; }

		void CopyToActiveScene(bool runtime);

		void ResetToEditorScene();

		bool IsPaused() const;
		void SetPaused(bool paused);

		void Step();
	private:
		Engine::Ref<Engine::Scene> m_ActiveScene;
		Engine::Ref<Engine::Scene> m_EditorScene;
		
	};
}