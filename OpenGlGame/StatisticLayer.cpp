#include "pch.h"

#include "StatisticLayer.h"
#include "Application.h"
#include "Keyboard.h"

#include "imgui.h"

namespace Game
{
	StatisticLayer::StatisticLayer() : Layer("StatistcLayer") {}

	void StatisticLayer::OnConstUpdate(const Time &ts)
	{
		m_Updates++;
	}

	void StatisticLayer::OnImGuiRender()
	{
		const auto &game   = Application::Get();
		const auto &window = game.GetWindow();

		if(!ImGui::Begin(
		                 "Fixed overlay",
		                 &m_Show,
		                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize
		                )) {}

		ImGui::Text("Fps: %.2f", 1.f / game.GetFrameTime().AsSeconds());
		ImGui::Text("Ups: %.2f", 1.f / m_LastUpdates);
		ImGui::Text("Frame time: %ims", game.GetFrameTime().AsMilliseconds());
		ImGui::Text("Elapsed Time: %.2fs", game.GetElapsedTime().AsSeconds());
		ImGui::Text("Updates: %u", m_LastUpdates);

		if(m_Clock.GetElapsedTime().AsSeconds() >= 1.f)
		{
			m_LastUpdates = m_Updates;
			m_Updates     = 0;
			m_Clock.Restart();
		}

		ImGui::End();
	}

	void StatisticLayer::OnUpdate()
	{
		if(Keyboard::IsKeyPressed(Key::F) && (Keyboard::IsKeyPressed(Key::LeftControl) ||
			Keyboard::IsKeyPressed(Key::RightControl)))
		{
			if(!m_Processed)
			{
				m_Processed = true;
				m_Show      = !m_Show;
				LOG_TRACE("DONE");
			}
		}
		m_Processed = false;
	}
}
