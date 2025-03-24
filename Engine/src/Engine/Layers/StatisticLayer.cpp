#include "pch.h"
#include "Engine/Layers/StatisticLayer.h"

#include "Engine/Core/Application.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Devices/Mouse.h"

#include "Engine/ImGui/ImGuiUtils.h"

#include <algorithm>
#include <array>
#include <float.h>
#include <imgui.h>

namespace Engine {
	constexpr uint32_t BUFFER_SIZE = 256;

	struct Statistics {
		std::array<float, BUFFER_SIZE> History{};

		uint32_t CurrentIndex = 0;
		double TotalHistory   = 0;
		float MinValue        = 0;
		float MaxValue        = 0;

		float GetAverage() const {
			return CurrentIndex == 0 ? 0.f : static_cast<float>(TotalHistory / CurrentIndex);
		}

		Statistics() {
			std::ranges::fill(History, 0.f);
		}

		void AddValue(float value) {
			TotalHistory -= History[0];
			std::move(History.begin() + 1, History.end(), History.begin());
			History[BUFFER_SIZE - 1] = value;
			TotalHistory += value;

			CurrentIndex = std::min(BUFFER_SIZE, CurrentIndex + 1);
		}

		void Draw(std::string_view name) {
			ImGui::PushID(name.data());

			constexpr float width = 0.f;
			const float height    = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;

			ImGui::PushID("Graph");
			auto color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
			color.w = 0.125;
			ImGui::PushStyleColor(ImGuiCol_FrameBg, color);

			ImGui::PlotLines("", History.data(), BUFFER_SIZE, 0, nullptr, FLT_MIN, FLT_MAX, {width, height});

			ImGui::PopStyleColor();
			ImGui::PopID();

			MinValue = *std::ranges::min_element(History);
			MaxValue = *std::ranges::max_element(History);

			ImGui::SameLine();
			ImGui::BeginGroup();

			Text("Min {}", MinValue);
			Text("Avg {}", GetAverage());
			Text("Max {}", MaxValue);

			ImGui::EndGroup();
			ImGui::PopID();
		}
	};

	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing;

	static Statistics s_FpsStat;

	StatisticLayer::StatisticLayer() : Layer("StatisticLayer") {}

	void StatisticLayer::OnAttach() {
		Layer::OnAttach();
	}


	void StatisticLayer::OnConstUpdate(const Time &timeStep) {
		m_ContUpdateCount++;
	}

	void StatisticLayer::OnImGuiRender() {
		if (!m_Show)
			return;

		const auto &app = Application::Get();
		const auto &window = app.GetWindow();
		const auto mousePos = Mouse::GetPosition(window);
		const auto windowPos = window.GetPos();
		const auto windowSize = window.GetSize();

		ImGui::SetNextWindowPos({static_cast<float>(windowPos.X), static_cast<float>(windowPos.Y)});
		ImGui::Begin("Fixed overlay", &m_Show, s_Flags);

		if (m_Clock.GetElapsedTime().AsSeconds() >= 1.f) {
			m_LastConstUpdateCount = m_ContUpdateCount;
			m_ContUpdateCount = 0;

			m_Clock.Restart();
		}

		s_FpsStat.AddValue(static_cast<float>(app.GetFrameTime().AsMilliseconds()));

		Text("Elapsed Time: {:.2f}s", app.GetElapsedTime().AsSeconds());
		Text("FPS: {:.2f}", 1.f / app.GetFrameTime().AsSeconds());
		Text("UPS: {:.2f}", 1.f / static_cast<float>(m_LastConstUpdateCount));
		Text("Frame Time: {}ms", app.GetFrameTime().AsMilliseconds());
		Text("Updates: {}", m_LastConstUpdateCount);
		Text("Mouse Position: {}", mousePos);
		Text("Window Position: {}", windowPos);
		Text("Window Size: {}", windowSize);

		s_FpsStat.Draw("FPS");

		ImGui::End();
	}

	void StatisticLayer::OnUpdate() {
		if(Keyboard::IsKeyPressed(Key::F) && (Keyboard::IsKeyPressed(Key::LeftControl) || Keyboard::IsKeyPressed(Key::RightControl))) {
			if(!m_Processed) {
				m_Processed = true;
				m_Show      = !m_Show;
			}
		}
		else
			m_Processed = false;
	}
}
