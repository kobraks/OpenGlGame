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

namespace {
	void DrawFrameBudgedBands(const ImVec2& topLeft, const ImVec2& graphSize, float targetRangeMs = 50.0f,
	                          float budget60fps = 1000.f / 60.f, float budget30fps = (1000.f / 60.f) * 2,
	                          ImU32 green = IM_COL32(0, 255, 0, 25), ImU32 yellow = IM_COL32(255, 255, 0, 25),
	                          ImU32 red = IM_COL32(255, 0, 0, 25)) {
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const float y60 = topLeft.y + graphSize.y * (1.f - budget60fps / targetRangeMs);
		const float y30 = topLeft.y + graphSize.y * (1.f - budget30fps / targetRangeMs);
		const float yBottom = topLeft.y + graphSize.y;

		const float left = topLeft.x;
		const float right = topLeft.x + graphSize.x;

		drawList->AddRectFilled(ImVec2(left, topLeft.y), ImVec2(right, y60), green);

		drawList->AddRectFilled(ImVec2(left, y60), ImVec2(right, y30), yellow);

		drawList->AddRectFilled(ImVec2(left, y30), ImVec2(right, yBottom), red);

		char label60[16], label30[16];
		snprintf(label60, sizeof(label60), "60 FPS");
		snprintf(label30, sizeof(label30), "30 FPS");

		drawList->AddText(ImVec2(left + 4, y60 - ImGui::GetTextLineHeight()), IM_COL32_WHITE, label60);
		drawList->AddText(ImVec2(left + 4, y30 - ImGui::GetTextLineHeight()), IM_COL32_WHITE, label30);

	}
}

namespace Engine {
	constexpr uint32_t BUFFER_SIZE = 256;

	struct Statistics {
		std::array<float, BUFFER_SIZE> History{};

		bool DynamicScale = false;
		float ScaleBy = 10.f;

		uint32_t CurrentIndex = 0;
		double TotalHistory = 0;
		float MinValue = 0;
		float MaxValue = 0;
		float TargetValue = 1000.f / 60.f; //60FPS
		Vector2f GraphScale{0.f, 50.f};

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

			if (DynamicScale) {
				GraphScale.X = std::min(MinValue, GraphScale.X) - ScaleBy;
				GraphScale.Y = std::max(MaxValue, GraphScale.Y) + ScaleBy;
			}
		}

		void Draw(std::string_view name) {
			ImGui::PushID(name.data());

			Text("{}", name);

			constexpr float width = 0.f;
			const float height = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;

			const ImVec2 graphTopLeft = ImGui::GetCursorScreenPos();

			ImGui::PushID("Graph");
			auto color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
			color.w = 0.125;
			ImGui::PushStyleColor(ImGuiCol_FrameBg, color);

			ImGui::PlotLines("", History.data(), BUFFER_SIZE, 0, nullptr, GraphScale.X, GraphScale.Y, {width, height});
			const ImVec2 graphSize = ImGui::GetItemRectSize();
			DrawFrameBudgedBands(graphTopLeft, graphSize, GraphScale.Y);

			ImGui::PopStyleColor();
			ImGui::PopID();

			const float yRatio = 1.f - TargetValue / GraphScale.Y;
			const float lineY = graphTopLeft.y + height * yRatio;

			const ImVec2 p1{graphTopLeft.x, lineY};
			const ImVec2 p2{graphTopLeft.x + graphSize.x, lineY};
			ImGui::GetWindowDrawList()->AddLine(p1, p2, IM_COL32(255, 0, 0, 100), 1.5f);

			MinValue = *std::ranges::min_element(History);
			MaxValue = *std::ranges::max_element(History);

			ImGui::SameLine();
			ImGui::BeginGroup();

			TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Min: {}", MinValue);
			TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Avg: {}", GetAverage());
			TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Max: {}", MaxValue);

			ImGui::EndGroup();
			ImGui::PopID();
		}
	};

	constexpr int s_Flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing;

	static Statistics s_FpsStat;
	static Statistics s_UpsStat;

	StatisticLayer::StatisticLayer() : Layer("StatisticLayer") {
		s_UpsStat.TargetValue = 60.f;
		s_UpsStat.GraphScale = Vector2f(0.f, 100.f);
		s_UpsStat.DynamicScale = true;
	}

	void StatisticLayer::OnAttach() {
		Layer::OnAttach();
	}


	void StatisticLayer::OnConstUpdate(const Time& timeStep) {
		++m_ConstUpdateCount;
		++m_InstantUps;
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
		ImGui::Begin("Fixed overlay", &m_Show, s_Flags);

		if (m_Clock.GetElapsedTime().AsSeconds() >= 1.f) {
			m_LastConstUpdateCount = m_ConstUpdateCount;
			m_ConstUpdateCount = 0;


			m_Clock.Restart();
		}

		s_FpsStat.AddValue(static_cast<float>(frameTime.AsMilliseconds()));
		s_UpsStat.AddValue(static_cast<float>(m_InstantUps / frameTime.AsSeconds()));

		Text("Elapsed Time: {:.2f}s", app.GetElapsedTime().AsSeconds());
		Text("Is VSync on: {}", window.IsVSync());
		Text("FPS: {:.2f}", 1.f / app.GetFrameTime().AsSeconds());
		Text("UPS (instant): {:.2f}", static_cast<float>(m_InstantUps) / frameTime.AsSeconds()); 
		Text("UPS (1s average): {:.2f}", static_cast<float>(m_LastConstUpdateCount)); 
		Text("Frame Time: {}ms", app.GetFrameTime().AsMilliseconds());
		Text("Updates: {}", m_LastConstUpdateCount);
		Text("Mouse Position: {}", mousePos);
		Text("Window Position: {}", windowPos);
		Text("Window Size: {}", windowSize);

		s_FpsStat.Draw("Frame Time");
		s_UpsStat.Draw("UPS");

		m_InstantUps = 0;

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
