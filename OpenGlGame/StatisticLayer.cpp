#include "pch.h"

#include <algorithm>

#include "StatisticLayer.h"
#include "Application.h"
#include "Keyboard.h"
#include "Mouse.h"

#include "imgui.h"
#include "ImGuiUtils.h"

namespace Game
{
	StatisticLayer::Metric::Metric()
	{
		std::fill(m_History, m_History + s_HISTORY_SAMPLES, 0.f);
	}

	void StatisticLayer::Metric::operator()(float time)
	{
		if(!m_First) m_Min = std::min(m_Min, time);
		else
		{
			m_First = false;
			m_Min   = time;
		}

		m_Max = std::max(m_Max, time);

		m_Total -= m_History[0];
		m_Total += time;
		
		memmove(m_History, m_History + 1, (s_HISTORY_SAMPLES - 1) * sizeof(float));

		m_History[s_HISTORY_SAMPLES - 1] = time;
		m_HistoryCount                   = std::min(s_HISTORY_SAMPLES, m_HistoryCount + 1);
	}

	void StatisticLayer::Metric::Draw() const
	{
		ImGui::Text("Frame Times: ");
		ImGui::SameLine();
		ImGui::PlotLines("", m_History, m_HistoryCount, s_HISTORY_SAMPLES - m_HistoryCount);
	}

	StatisticLayer::StatisticLayer() : Layer("StatistcLayer") {}
	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;

	void StatisticLayer::OnConstUpdate(const Time &ts)
	{
		m_Updates++;
	}

	void StatisticLayer::OnImGuiRender()
	{
		if(!m_Show) return;

		const auto &game    = Application::Get();
		const auto &window  = game.GetWindow();
		const auto mousePos = Mouse::GetPosition(window);

		const auto guard = ImGuiUniqueGuard("Fixed overlay", m_Show, s_Flags);
		m_Metric(game.GetFrameTime().AsMilliseconds());
		
		Text("Fps: {:.2f}", 1.f / game.GetFrameTime().AsSeconds());
		Text("Ups: {:.2f}", 1.f / m_LastUpdates);
		Text("Frame Time: {}ms", game.GetFrameTime().AsMilliseconds());
		Text("Elapsed Time: {:.2f}s", game.GetElapsedTime().AsSeconds());
		Text("Updates: {}", m_LastUpdates);
		Text("Mouse Position: {}, {}", mousePos.X, mousePos.Y);

		m_Metric.Draw();
		
		Text("Min frame Time: {:.2f}ms", m_Metric.Min());
		Text("Max frame Time: {:.2f}ms", m_Metric.Max());
		Text("Avr frame Time: {:.2f}ms", m_Metric.Avr());

		if(m_Clock.GetElapsedTime().AsSeconds() >= 1.f)
		{
			m_LastUpdates = m_Updates;
			m_Updates     = 0;
			m_Clock.Restart();
		}
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
			}
		}
		else m_Processed = false;
	}
}
