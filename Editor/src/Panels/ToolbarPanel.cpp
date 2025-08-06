#include "ToolbarPanel.h"

#include "Engine/Core/Image.h"

namespace Editor {
	ToolbarPanel::ToolbarPanel() {
		m_IconPlay = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/PlayButton.png"));
		m_IconPause = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/PauseButton.png"));
		m_IconStep = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/StepButton.png"));
		m_IconSimulate = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/SimulateButton.png"));
		m_IconStop = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/StopButton.png"));
	}

	void ToolbarPanel::OnImGuiRender() {
		const bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		const bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		const bool hasPauseButton = m_SceneState != SceneState::Edit;

		const int buttonCount = hasPlayButton + hasSimulateButton + hasPauseButton + 1;

		constexpr float windowHeight = 32;

		constexpr float spacing = 4.0f;
		constexpr float buttonSize = windowHeight - spacing;

		const float windowWidth = buttonSize * static_cast<float>(buttonCount) + static_cast<float>(buttonCount - 1) *
			spacing;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		const auto& colors = ImGui::GetStyle().Colors;

		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::SetNextWindowSize({ windowWidth, windowHeight + spacing * 2 });
		// ImGui::SetNextWindowPos();
		ImGui::Begin("##toolbar", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		auto tintColor = ImVec4(1, 1, 1, 1.f);

		if (!m_Enabled)
			tintColor.w = 0.5f;

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.2f) - (buttonSize * 0.5f));

		if (hasPlayButton) {
			const auto& icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
				? m_IconPlay
				: m_IconStop;
			if (DrawButtonIcon(icon, "##PlayButton", buttonSize, tintColor) && m_Enabled) {
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) {
					if (m_OnPlay)
						m_OnPlay();
				}
				else if (m_SceneState == SceneState::Play) {
					if (m_OnStop)
						m_OnStop();
				}
			}
		}

		if (hasSimulateButton) {
			if (hasPlayButton)
				ImGui::SameLine(0, spacing);

			const auto& icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
				? m_IconSimulate
				: m_IconStop;
			if (DrawButtonIcon(icon, "##SimulateButton", buttonSize, tintColor) && m_Enabled) {
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) {
					if (m_OnSimulate)
						m_OnSimulate();
				}
				else if (m_SceneState == SceneState::Simulate) {
					if (m_OnStop)
						m_OnStop();
				}
			}
		}

		if (hasPauseButton) {
			const bool isPaused = m_Paused;
			ImGui::SameLine(0, spacing);
			{
				const auto& icon = m_IconPause;
				if (DrawButtonIcon(icon, "##PauseButton", buttonSize, tintColor) && m_Enabled) {
					if (m_OnPause)
						m_OnPause();
				}
			}

			if (isPaused) {
				ImGui::SameLine(0, spacing);
				{
					const auto& icon = m_IconStep;
					if (DrawButtonIcon(icon, "##StepButton", buttonSize, tintColor) && m_Enabled) {
						if (m_OnStep) {
							m_OnStep();
						}
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}

	bool ToolbarPanel::DrawButtonIcon(const Engine::Ref<Engine::Texture>& icon, const std::string& name, float size, const ImVec4& tintColor) {
		return ImGui::ImageButton(name.c_str(), icon->RendererID(), {size, size}, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, tintColor);
	}
}
