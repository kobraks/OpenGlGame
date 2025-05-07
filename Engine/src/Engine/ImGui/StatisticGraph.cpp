#include "pch.h"
#include "StatisticGraph.h"

#include "Engine/ImGui/ImGuiUtils.h"

#include <imgui.h>
#include <algorithm>
#include <limits>
#include <fmt/format.h>

namespace Engine {
	StatisticGraph::StatisticGraph(const std::string& name, uint64_t bufferSize) : m_Name(name), m_History(bufferSize, 0.0f), m_OrderedHistory(bufferSize, 0.0f), m_BufferSize(bufferSize) {
	}

	void StatisticGraph::AddValue(float value) {
		m_Total -= m_History[m_HeadIndex];

		//Insert new value
		m_History[m_HeadIndex] = value;
		m_Total += value;

		//Advance head
		m_HeadIndex = (m_HeadIndex + 1) % m_BufferSize;
		m_Count = std::min(m_Count + 1, m_BufferSize);

		m_Dirty = true;

		if (m_DynamicScale) {
			m_Scale.X = std::max(0.0f, m_Min - m_ScaleMargin);
			m_Scale.Y = m_Max + m_ScaleMargin;
		}
	}

	void StatisticGraph::Draw() {
		Draw(m_Name);
	}

	void StatisticGraph::Draw(std::string_view label) {
		if (m_Dirty)
			RecalculateMinMax();

		RebuildOrderedBuffer();

		ImGui::PushID(label.data());
		Text("{}", label);

		auto drawList = ImGui::GetWindowDrawList();
		const ImVec2 graphTopLeft = ImGui::GetCursorScreenPos();

		ImGui::PushID("Graph");
		ImGui::BeginGroup();

		DrawGraph();

		auto graphSize = ImGui::GetItemRectSize();

		DrawBands(graphTopLeft, graphSize, drawList);

		if (m_TargetVisible) {
			DrawTargetLine(graphTopLeft, graphSize, drawList);
		}

		ImGui::EndGroup();
		ImGui::PopID();

		ImGui::PopID();
	}

	void StatisticGraph::Reset() {
		std::ranges::fill(m_History, 0.0f);

		m_HeadIndex = 0;
		m_Count = 0;
		m_Total = 0.0f;
		m_Min = m_Max = 0.0f;
		m_Dirty = true;
	}

	void StatisticGraph::AddBand(float threshold, uint32_t color, const char* label) {
		m_Bands.emplace_back(threshold, color, label);
	}

	const Band& StatisticGraph::GetBand(size_t index) const {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		return m_Bands[index];
	}

	Band& StatisticGraph::GetBand(size_t index) {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		return m_Bands[index];
	}

	void StatisticGraph::RemoveBand(size_t index) {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		m_Bands.erase(m_Bands.begin() + index);
	}

	void StatisticGraph::ClearBands() {
		m_Bands.clear();
	}

	float StatisticGraph::GetAverage() const {
		return m_Count == 0 ? 0.0f : m_Total / static_cast<float>(m_Count);
	}

	void StatisticGraph::RecalculateMinMax() {
		if (m_Count == 0) {
			m_Min = m_Max = 0;
			return;
		}

		float min = std::numeric_limits<float>::max();
		float max = std::numeric_limits<float>::lowest();

		for (uint64_t i = 0; i < m_Count; ++i) {
			const uint64_t index = (m_HeadIndex + i) % m_BufferSize;
			const float value = m_History[index];

			min = std::min(min, value);
			max = std::max(max, value);
		}

		m_Min = min;
		m_Max = max;

		m_Dirty = false;
	}

	void StatisticGraph::RebuildOrderedBuffer() {
		for (uint64_t i = 0; i < m_Count; ++i) {
			const uint64_t index = (m_HeadIndex + i) % m_BufferSize;
			m_OrderedHistory[i] = m_History[index];
		}
	}

	void StatisticGraph::DrawGraph() const {
		const float height = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;

		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
		color.w = 0.125f;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, color);

		ImGui::PlotLines("", m_OrderedHistory.data(), static_cast<int>(m_Count), 0, nullptr, m_Scale.X, m_Scale.Y, { 0.f, height });
		
		ImGui::PopStyleColor();
	}

	void StatisticGraph::DrawBands(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const {
		if (m_Bands.empty()) return;

		auto sortedBands = m_Bands;
		std::ranges::sort(sortedBands, [](const Band& a, const Band& b) {
			return a.Threshold < b.Threshold;
		});

		auto ToY = [&](float value) -> float {
			const float ratio = 1.f - ((value - m_Scale.X) / (m_Scale.Y - m_Scale.X));
			return topLeft.y + graphSize.y * std::clamp(ratio, 0.f, 1.f);
			};

		float prevY = topLeft.y + graphSize.y;

		for (const auto& band : sortedBands) {
			float bandY = ToY(band.Threshold);
			drawList->AddRectFilled({ topLeft.x, bandY }, { topLeft.x + graphSize.x, prevY }, band.Color);

			if (band.Label) {
				drawList->AddText({ topLeft.x + 4, prevY - ImGui::GetTextLineHeight() - 2 }, IM_COL32_WHITE, band.Label);
			}

			prevY = bandY;
		}

		if (prevY > topLeft.y) {
			drawList->AddRectFilled({ topLeft.x, topLeft.y }, { topLeft.x + graphSize.x, prevY }, IM_COL32(100, 100, 100, 25));
		}
	}

	void StatisticGraph::DrawSideStats() const {
		ImGui::SameLine();
		ImGui::BeginGroup();
		TextColored({ 0.3f, 1.f, 0.3f, 1.f }, "Min: {:.2f} {}", m_Min, m_Units);
		TextColored({ 0.3f, 0.7f, 1.0f, 1.f }, "Avg: {:.2f} {}", GetAverage(), m_Units);
		TextColored({ 1.0f, 0.3f, 0.3f, 1.f }, "Max: {:.2f} {}", m_Max, m_Units);
		ImGui::EndGroup();
	}

	void StatisticGraph::DrawTargetLine(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const {
		//draw target line
		const float scaleRange = m_Scale.Y - m_Scale.X;
		const float yRatio = 1.f - (m_Target - m_Scale.X) / scaleRange;
		const float lineY = topLeft.y + graphSize.y * yRatio;

		ImVec2 p1{ topLeft.x, lineY };
		ImVec2 p2{ topLeft.x + graphSize.x, lineY };

		drawList->AddLine(p1, p2, IM_COL32(255, 0, 0, 90), 1.5f);
		drawList->AddText({ p1.x + 4, p1.y - ImGui::GetTextLineHeight() }, IM_COL32(255, 255, 255, 180), fmt::format("Target: {:.2f} {}", m_Target, m_Units).c_str());
	}
}
