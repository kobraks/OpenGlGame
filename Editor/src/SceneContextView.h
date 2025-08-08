#pragma once
#include "SceneState.h"
#include "Engine/Scene/Scene.h"

namespace Editor {
	class SceneContext;
	class SceneStateController;

	class SceneContextView {
	public:
		SceneContextView() = default;
		SceneContextView(SceneContext* context, SceneStateController* controller);

		operator bool() const { return m_Context && m_Controller; }

		Engine::Ref<Engine::Scene> GetEditorScene() const;
		Engine::Ref<Engine::Scene> GetActiveScene() const;

		SceneState GetState() const;
		bool IsPaused() const;

		bool IsEditMode() const { return GetState() == SceneState::Edit; }
		bool IsPlayMode() const { return GetState() == SceneState::Play; }
		bool IsSimulateMode() const { return GetState() == SceneState::Simulate; }

	private:
		SceneContext* m_Context = nullptr;
		SceneStateController* m_Controller = nullptr;
	};
}