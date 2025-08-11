#pragma once

#include "../SceneState.h" //TODO fix include path

#include "Engine/Renderer/Texture.h"

#include <functional>

#include <ImGui/imgui.h>

namespace Editor {
	class ToolbarPanel {
	public:
		using Callback = std::function<void()>;

		ToolbarPanel();

		void SetEnabled(bool enabled) { m_Enabled = enabled; }

		void SetState(SceneState state) { m_SceneState = state; }
		void SetPaused(bool paused) { m_Paused = paused; }

		[[nodiscard]] SceneState GetState() const { return m_SceneState; }
		[[nodiscard]] bool IsPaused() const { return m_Paused; }

		void SetCallbackPlay(Callback cb) { m_OnPlay = std::move(cb); }
		void SetCallbackPause(Callback cb) { m_OnPause = std::move(cb); }
		void SetCallbackStop(Callback cb) { m_OnStop = std::move(cb); }
		void SetCallbackSimulate(Callback cb) { m_OnSimulate = std::move(cb); }
		void SetCallbackStep(Callback cb) { m_OnStep = std::move(cb); }

		void OnImGuiRender();

	private:
		static bool DrawButtonIcon(const Engine::Ref<Engine::Texture>& icon, const std::string& name, float size, const ImVec4& tintColor);

	private:
		SceneState m_SceneState = SceneState::Edit;

		bool m_Enabled = true;
		bool m_Paused = false;

		Engine::Ref<Engine::Texture> m_IconPlay;
		Engine::Ref<Engine::Texture> m_IconPause;
		Engine::Ref<Engine::Texture> m_IconStop;
		Engine::Ref<Engine::Texture> m_IconSimulate;
		Engine::Ref<Engine::Texture> m_IconStep;
		
		Callback m_OnPlay;
		Callback m_OnPause;
		Callback m_OnStop;
		Callback m_OnSimulate;
		Callback m_OnStep;
	};
}
