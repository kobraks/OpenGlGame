#include "ToolbarPanel.h"

#include "Engine/Core/Image.h"
#include "Engine/ImGui/ImGuiScoped.h"

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

		const int buttonCount = hasPlayButton + hasSimulateButton + hasPauseButton + (hasPauseButton ? m_Paused : 0);

		constexpr float windowHeight = 32.f;

		constexpr float spacing = 4.0f;
		constexpr float buttonSize = windowHeight - spacing;
		constexpr float buttonActualSize = buttonSize + 2.f;
		const float windowWidth = (buttonActualSize) * static_cast<float>(buttonCount) + static_cast<float>(buttonCount + 1) * (spacing);

		Engine::ScopedStyleVar styleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f),
			ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f),
			ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));

		const auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];

		Engine::ScopedStyleColor styleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f),
			ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f),
			ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::SetNextWindowSize({ windowWidth, windowHeight + spacing * 2 });
		// ImGui::SetNextWindowPos();
		ImGui::Begin("##toolbar", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		const auto tintColor = ImVec4(1.f, 1.f, 1.f, !m_Enabled ? 0.5f : 1.f);

		// ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.2f) - (buttonSize * 0.5f));

		ImGui::SetCursorPosY(spacing * 1.5f);
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - buttonActualSize * static_cast<float>(buttonCount) - 1.5f * spacing));
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

		ImGui::End();
	}

	bool ToolbarPanel::DrawButtonIcon(const Engine::Ref<Engine::Texture>& icon, const std::string& name, float size, const ImVec4& tintColor) {
		const ImVec2 imageSize{ size, size };
		constexpr ImVec2 uv0{ 0.0f, 0.0f };
		constexpr ImVec2 uv1{ 1.0f, 1.0f };
		constexpr ImVec4 bgColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		return ImGui::ImageButton(name.c_str(), icon->RendererID(), imageSize, uv0, uv1, bgColor, tintColor);
	}
}
