#include "pch.h"

#include <algorithm>

#include "StatisticLayer.h"
#include "Application.h"
#include "Keyboard.h"
#include "Mouse.h"

#include "imgui.h"
#include "ImGuiGuard.h"
#include "ImGuiUtils.h"

namespace Game
{
	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoInputs;


	StatisticLayer::StatisticLayer() : Layer("StatistcLayer") {}
	void StatisticLayer::OnAttach()
	{
		Application::Get().RegisterShortcut(Shortcut([this](){ m_Show = !m_Show; }, Key::F, Key::LeftControl));
		Application::Get().RegisterShortcut(Shortcut([this](){ m_Show = !m_Show; }, Key::F, Key::RightControl));
	}

	void StatisticLayer::OnConstUpdate(const Time &ts)
	{
		m_Updates++;
	}

	void StatisticLayer::OnImGuiRender()
	{
		if(!m_Show)
			return;

		const auto &game    = Application::Get();
		const auto &window  = game.GetWindow();
		const auto mousePos = Mouse::GetPosition(window);
		const auto windowPos = window.GetPosition();
		
		ImGuiWindowPosition position{windowPos};
		
		const auto guard = ImGuiUniqueGuard<ImGuiMainWindow>(ImGuiMainWindowProps{"Fixed overlay", m_Show, s_Flags}, position);

		if(m_Clock.GetElapsedTime().AsSeconds() >= 1.f)
		{
			m_LastUpdates = m_Updates;
			m_Updates     = 0;

			m_Clock.Restart();
		}

		Text("Fps: {:.2f}", 1.f / game.GetFrameTime().AsSeconds());
		Text("Ups: {:.2f}", 1.f / m_LastUpdates);
		Text("Frame Time: {}ms", game.GetFrameTime().AsMilliseconds());
		Text("Elapsed Time: {:.2f}s", game.GetElapsedTime().AsSeconds());
		Text("Updates: {}", m_LastUpdates);
		Text("Mouse Position: {}, {}", mousePos.X, mousePos.Y);
		Text("Window Position: {}, {}", windowPos.X, windowPos.Y);
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
