#include "pch.h"
#include "Engine/Layers/StatisticLayer.h"

#include "Engine/Core/Application.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Devices/Mouse.h"

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

		void Draw(const std::string &name) {
			ImGui::PushID(name.c_str());

			constexpr float width = 0.f;
			const float height    = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;

			ImGui::PlotLines("", History.data(), BUFFER_SIZE, 0, nullptr, FLT_MIN, FLT_MAX, {width, height});

			MinValue = *std::ranges::min_element(History);
			MaxValue = *std::ranges::max_element(History);

			ImGui::SameLine();
			ImGui::BeginGroup();

			ImGui::Text(fmt::format("Min {}", MinValue).c_str());
			ImGui::Text(fmt::format("Avg {}", GetAverage()).c_str());
			ImGui::Text(fmt::format("Max {}", MaxValue).c_str());

			ImGui::EndGroup();
			ImGui::PopID();
		}
	};

	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs;

	static Statistics s_FpsStat;

	StatisticLayer::StatisticLayer() : Layer("StatisticLayer") {}

	void StatisticLayer::OnAttach() {
		Layer::OnAttach();
	}


	void StatisticLayer::OnConstUpdate(const Time &timeStep) {
		m_Updates++;
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
			m_LastUpdates = m_Updates;
			m_Updates = 0;

			m_Clock.Restart();
		}

		// s_FpsStat.AddValue();

		s_FpsStat.Draw("FPS");

		ImGui::End();
	}

	void StatisticLayer::OnUpdate() {
		if(Keyboard::IsKeyPressed(Key::F, Application::Get().GetWindow()) && (
			Keyboard::IsKeyPressed(Key::LeftControl, Application::Get().GetWindow()) || Keyboard::IsKeyPressed(
				 Key::RightControl,
				 Application::Get().GetWindow()
				))) {
			if(!m_Processed) {
				m_Processed = true;
				m_Show      = !m_Show;
			}
		}
		else
			m_Processed = false;
	}
}
