#pragma once
#include "SceneContext.h"

namespace Editor {
	class SceneStateController {
	public:
		enum class State {
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		SceneStateController() = default;
		explicit SceneStateController(SceneContext* context);

		void BindContext(SceneContext* context) { m_Context = context; }

		SceneContext* GetContext() const { return m_Context; }

		void Play();
		void Simulate();
		void Stop();
		void Pause();

		State GetState() const { return m_State; }
		bool IsPaused() const { return m_Context && m_Context->IsPaused(); }

	private:
		State m_State = State::Edit;
		SceneContext* m_Context = nullptr;
	};

}