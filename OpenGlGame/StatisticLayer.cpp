#include "pch.h"

#include <algorithm>

#include "StatisticLayer.h"
#include "Application.h"
#include "Keyboard.h"
#include "Mouse.h"

#include "imgui.h"

#include <float.h>

#include "ImGuiGuard.h"
#include "ImGuiUtils.h"

namespace Game
{
	constexpr uint32_t BUFFER_SIZE = 256;

	struct Statistics
	{
		std::array<float, BUFFER_SIZE> History;
		uint32_t CurrentIndex = 0;
		double TotalInHistory = 0;
		float MinimalValue    = 0;
		float MaxValue        = 0;

		float GetAverage() const
		{
			return CurrentIndex == 0 ? 0.f : static_cast<float>(TotalInHistory / CurrentIndex);
		}

		Statistics()
		{
			std::ranges::fill(History, 0.f);
		}

		void AddValue(float value)
		{
			TotalInHistory -= History[0];
			std::move(History.begin() + 1, History.end(), History.begin());
			History[BUFFER_SIZE - 1] = value;
			TotalInHistory += value;

			CurrentIndex = std::min(BUFFER_SIZE, CurrentIndex + 1);
		}

		void Draw(const std::string &name)
		{
			ImGui::PushID(name.c_str());
		
			const float width = 0;
			const float height = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;
			
			ImGui::PlotLines("", History.data(), BUFFER_SIZE, 0, 0, FLT_MIN, FLT_MAX, {width, height});

			MinimalValue = *std::min_element(History.begin(), History.end());
			MaxValue     = *std::max_element(History.begin(), History.end());

			ImGui::SameLine();
			ImGui::BeginGroup();

			ImGui::Text(fmt::format("Min {}", MinimalValue).c_str());
			ImGui::Text(fmt::format("Avg {}", GetAverage()).c_str());
			ImGui::Text(fmt::format("Max {}", MaxValue).c_str());

			ImGui::EndGroup();
			ImGui::PopID();
		}
	};

	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoInputs;


	static Statistics s_FpsStat;

	StatisticLayer::StatisticLayer() : Layer("StatistcLayer") {}

	void StatisticLayer::OnAttach()
	{
		Application::Get().RegisterShortcut(Shortcut([this]() { m_Show = !m_Show; }, Key::F, Key::LeftControl));
		Application::Get().RegisterShortcut(Shortcut([this]() { m_Show = !m_Show; }, Key::F, Key::RightControl));
		// Application::Get().RegisterShortcut(Shortcut([this](){m_ShowMetric = !m_ShowMetric}, Key::))
	}

	void StatisticLayer::OnConstUpdate(const Time &ts)
	{
		m_Updates++;
	}

	void StatisticLayer::OnImGuiRender()
	{
		if(!m_Show)
			return;

		const auto &game      = Application::Get();
		const auto &window    = game.GetWindow();
		const auto mousePos   = Mouse::GetPosition(window);
		const auto windowPos  = window.GetPosition();
		const auto windowSize = window.GetSize();

		ImGuiWindowPosition position{windowPos};

		const auto guard = ImGuiUniqueGuard<ImGuiMainWindow>(ImGuiMainWindowProps{"Fixed overlay", m_Show, s_Flags}, position);

		if(m_Clock.GetElapsedTime().AsSeconds() >= 1.f)
		{
			m_LastUpdates = m_Updates;
			m_Updates     = 0;

			m_Clock.Restart();
		}

		s_FpsStat.AddValue(static_cast<float>(game.GetFrameTime().AsMilliseconds()));

		Text("Fps: {:.2f}", 1.f / game.GetFrameTime().AsSeconds());
		Text("Ups: {:.2f}", 1.f / m_LastUpdates);
		Text("Frame Time: {}ms", game.GetFrameTime().AsMilliseconds());
		Text("Elapsed Time: {:.2f}s", game.GetElapsedTime().AsSeconds());
		Text("Updates: {}", m_LastUpdates);
		Text("Mouse Position: {}, {}", mousePos.X, mousePos.Y);
		Text("Window Position: {}, {}", windowPos.X, windowPos.Y);
		Text("Window size {}x{}", windowSize.Width, windowSize.Height);

		s_FpsStat.Draw("Fps");
	}

	void StatisticLayer::OnUpdate()
	{
		// if(Keyboard::IsKeyPressed(Key::F) && (Keyboard::IsKeyPressed(Key::LeftControl) || Keyboard::IsKeyPressed(Key::RightControl)))
		// {
		// 	if(!m_Processed)
		// 	{
		// 		m_Processed = true;
		// 		m_Show      = !m_Show;
		// 	}
		// }
		// else
		// 	m_Processed = false;
	}
}
