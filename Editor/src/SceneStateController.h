#pragma once
#include "SceneContext.h"
#include "SceneState.h"

namespace Editor {
	class SceneStateController {
	public:
		SceneStateController() = default;
		explicit SceneStateController(SceneContext* context);

		void BindContext(SceneContext* context) { m_Context = context; }

		SceneContext* GetContext() const { return m_Context; }

		void Play();
		void Simulate();
		void Stop();
		void Pause();

		SceneState GetState() const { return m_State; }
		bool IsPaused() const { return m_Context && m_Context->IsPaused(); }

	private:
		SceneState m_State = SceneState::Edit;
		SceneContext* m_Context = nullptr;
	};

}