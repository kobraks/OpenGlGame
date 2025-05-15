#include "pch.h"
#include "Engine/Layers/StatisticLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Vector2.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Devices/Mouse.h"

#include "Engine/ImGui/ImGuiUtils.h"

#include <algorithm>
#include <array>
#include <float.h>
#include <imgui.h>

#include "Engine/Devices/Monitor.h"

namespace Engine {
	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing;

	StatisticLayer::StatisticLayer() : Layer("StatisticLayer") {
		constexpr float targetFPS60 = 60.f;
		constexpr float targetFPS30 = 30.f;
		constexpr float criticalCutoffFPS = 20.0f;

		constexpr float budget60FPS = 1000.f / targetFPS60;
		constexpr float budget30FPS = 1000.f / targetFPS30;
		constexpr float budget20FPS = 1000.f / criticalCutoffFPS;

		m_FrameGraph = MakeScope<StatisticGraph>("Frame Time");
		m_UpdateGraph = MakeScope<StatisticGraph>("UPS");

		m_FrameGraph->SetTarget(1000.f / 60.f);
		m_FrameGraph->SetScale(0.0f, 50.f);
		m_FrameGraph->SetUnit("ms");

		m_FrameGraph->AddBand(budget60FPS, IM_COL32(0, 255, 0, 25), ">= 60 FPS");
		m_FrameGraph->AddBand(budget30FPS, IM_COL32(255, 255, 0, 25), ">= 30 FPS");
		m_FrameGraph->AddBand(budget20FPS, IM_COL32(255, 0, 0, 25), "< 30 FPS");

		m_UpdateGraph->SetTarget(60.f);
		m_UpdateGraph->SetScale(0, 120);
		m_UpdateGraph->SetUnit("UPS");
		m_UpdateGraph->EnableSideStats(false);

		m_UpdateGraph->AddBand(55.f, IM_COL32(0, 150, 255, 25), "Under 60 UPS");
		m_UpdateGraph->AddBand(65.f, IM_COL32(0, 255, 0, 25), "Stable (60UPS)");
		m_UpdateGraph->AddBand(90.f, IM_COL32(255, 255, 0, 25), "Catch-up");
		m_UpdateGraph->AddBand(999.f, IM_COL32(255, 0, 0, 25), "Spikes");

		m_GraphGroup.AddGraph("fps", m_FrameGraph);
		m_GraphGroup.AddGraph("ups", m_UpdateGraph);
	}

	void StatisticLayer::OnAttach() {
		Layer::OnAttach();
	}

	void StatisticLayer::OnUpdate() {
		const auto& app = Application::Get();
		const auto frameTime = app.GetFrameTime();

		m_FrameGraph->AddValue(static_cast<float>(frameTime.AsMilliseconds()));
		float ups = frameTime != Time::Zero ? static_cast<float>(m_InstantUpdateCount) / frameTime.AsSeconds() : 0.0f;
		ups = std::clamp(ups, 0.0f, 120.0f);
		m_UpdateGraph->AddValue(ups);

		m_InstantUpdateCount = 0;

		Layer::OnUpdate();
	}


	void StatisticLayer::OnConstUpdate(const Time& timeStep) {
		++m_ConstUpdateCount;
		++m_InstantUpdateCount;
	}

	void StatisticLayer::OnImGuiRender() {
		if (!m_Show)
			return;

		const auto& app = Application::Get();
		const auto& window = app.GetWindow();
		const auto mousePos = Mouse::GetPosition(window);
		const auto windowPos = window.GetPos();
		const auto windowSize = window.GetSize();
		const auto frameTime = app.GetFrameTime();

		ImGui::SetNextWindowPos({static_cast<float>(windowPos.X), static_cast<float>(windowPos.Y)});
		ImGui::Begin("##Fixed overlay", &m_Show, s_Flags);

		if (m_Clock.GetElapsedTime().AsSeconds() >= 1.f) {
			m_LastConstUpdateCount = m_ConstUpdateCount;
			m_ConstUpdateCount = 0;


			m_Clock.Restart();
		}

		// m_FrameGraph->AddValue(static_cast<float>(frameTime.AsMilliseconds()));
		// m_UpdateGraph->AddValue(static_cast<float>(m_InstantUpdateCount / frameTime.AsSeconds()));

		Text("Elapsed Time: {:.2f}s", app.GetElapsedTime().AsSeconds());
		Text("Is VSync on: {}", window.IsVSync());
		Text("FPS: {:.2f}", 1.f / app.GetFrameTime().AsSeconds());
		Text("UPS (instant): {:.2f}", static_cast<float>(m_InstantUpdateCount) / frameTime.AsSeconds()); 
		Text("UPS (1s average): {:.2f}", static_cast<float>(m_LastConstUpdateCount)); 
		Text("Frame Time: {}ms", app.GetFrameTime().AsMilliseconds());
		Text("Updates: {}", m_LastConstUpdateCount);
		Text("Mouse Position: {}", mousePos);
		Text("Window Position: {}", windowPos);
		Text("Window Size: {}", windowSize);

		// m_InstantUps = 0;

		m_GraphGroup.DrawAll();

		ImGui::End();
	}

	void StatisticLayer::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(StatisticLayer::OnKeyPressEvent));
	}

	bool StatisticLayer::OnKeyPressEvent(const KeyPressedEvent& event) {
		if (event.IsRepeat())
			return false;

		const bool control = Keyboard::IsKeyPressed(Key::LeftControl) || Keyboard::IsKeyPressed(Key::RightControl);

		if (control && event.GetKeyCode() == Key::F) {
			m_Show = !m_Show;
		}

		return false;
	}
}
