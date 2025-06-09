#include "pch.h"
#include "StatisticGraph.h"

#include "Engine/ImGui/ImGuiScoped.h"
#include "Engine/ImGui/ImGuiWidgets.h"

#include <imgui.h>
#include <algorithm>
#include <limits>
#include <fmt/format.h>

namespace Engine {
	StatisticGraph::StatisticGraph(const std::string& name, uint64_t bufferSize) : m_Name(name), m_History(bufferSize, 0.0f), m_PlotBuffer(bufferSize, 0.0f), m_BufferSize(bufferSize) {
	}

	void StatisticGraph::AddValue(float value) {
		m_Total -= m_History[m_HeadIndex];

		//Insert new value
		m_History[m_HeadIndex] = value;
		m_Total += value;

		//Advance head
		m_HeadIndex = (m_HeadIndex + 1) % m_BufferSize;
		m_Count = std::min(m_Count + 1, m_BufferSize);

		m_HistoryDirty = true;

		if (m_DynamicScale) {
			m_ScaleMin = std::max(0.0f, m_Min - m_ScaleMargin);
			m_ScaleMax = m_Max + m_ScaleMargin;
		}
	}

	void StatisticGraph::Draw() {
		Draw(m_Name);
	}

	void StatisticGraph::Draw(std::string_view label) {
		if (m_HistoryDirty)
			RecalculateMinMax();

		RebuildOrderedBuffer();

		ScopedID drawID(label);
		TextUnformatted("{}", label);

		auto drawList = ImGui::GetWindowDrawList();
		const ImVec2 graphTopLeft = ImGui::GetCursorScreenPos();

		{
			ScopedID graphID("Graph");
			ScopedGroup group;

			DrawGraph();

			auto graphSize = ImGui::GetItemRectSize();

			DrawValueBands(graphTopLeft, graphSize, drawList);

			if (m_TargetVisible) {
				DrawTargetLine(graphTopLeft, graphSize, drawList);
			}
		}

		if (m_ShowSideStats) {
			DrawSideStats();
		}
	}

	void StatisticGraph::Reset() {
		std::ranges::fill(m_History, 0.0f);

		m_HeadIndex = 0;
		m_Count = 0;
		m_Total = 0.0f;
		m_Min = m_Max = 0.0f;
		m_HistoryDirty = true;
	}

	void StatisticGraph::AddBand(float threshold, uint32_t color, const char* label) {
		m_Bands.emplace_back(threshold, color, label);
		m_BandSortedDirty = true;
	}

	const Band& StatisticGraph::GetBand(size_t index) const {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		return m_Bands[index];
	}

	void StatisticGraph::ModifyBand(size_t index, float newThreshold, uint32_t newColor, const char* newLabel) {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		Band& band = m_Bands[index];
		band.Threshold = newThreshold;
		band.Color = newColor;
		band.Label = newLabel;

		m_BandSortedDirty = true;
	}

	void StatisticGraph::RemoveBand(size_t index) {
		ENGINE_ASSERT(index < m_Bands.size());
		if (index >= m_Bands.size())
			throw std::out_of_range("No band at given index");

		m_Bands.erase(m_Bands.begin() + index);
		m_BandSortedDirty = true;
	}

	void StatisticGraph::ClearBands() {
		m_Bands.clear();
		m_BandSortedDirty = true;
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

		m_HistoryDirty = false;
	}

	void StatisticGraph::RebuildOrderedBuffer() {
		const uint64_t targetIndex = m_BufferSize - 1;

		for (uint64_t i = 0; i < m_Count; ++i) {
			const uint64_t historyIndex = (m_HeadIndex + m_Count - 1 - i + m_BufferSize) % m_BufferSize;
			m_PlotBuffer[targetIndex - i] = m_History[historyIndex];
		}
		for (uint64_t i = 0; i < m_BufferSize - m_Count; ++i) {
			m_PlotBuffer[i] = std::numeric_limits<float>::quiet_NaN();
		}
	}

	const std::vector<const Band*>& StatisticGraph::GetSortedBands() const {
		if (m_BandSortedDirty) {
			m_SortedBandsCache.clear();
			m_SortedBandsCache.reserve(m_Bands.size());

			for (const auto& band : m_Bands) {
				m_SortedBandsCache.emplace_back(&band);
			}

			std::ranges::sort(m_SortedBandsCache, [](const Band* a, const Band* b) { return a->Threshold < b->Threshold; });

			m_BandSortedDirty = false;
		}

		return m_SortedBandsCache;
	}

	void StatisticGraph::DrawGraph() const {
		const float height = (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.y) * 3;

		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
		color.w = 0.125f;
		ScopedStyleColor bgColor(ImGuiCol_FrameBg, color);

		ImGui::PlotLines("##Plot", m_PlotBuffer.data(), static_cast<int>(m_BufferSize), 0, nullptr, m_ScaleMin, m_ScaleMax, { 0.f, height });
	}

	void StatisticGraph::DrawValueBands(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const {
		if (m_Bands.empty()) return;

		auto& sortedBands = GetSortedBands();

		auto ToY = [&](float value) -> float {
			const float ratio = 1.f - ((value - m_ScaleMin) / (m_ScaleMax - m_ScaleMin));
			return topLeft.y + graphSize.y * std::clamp(ratio, 0.f, 1.f);
			};

		float prevY = topLeft.y + graphSize.y;

		for (const auto& band : sortedBands) {
			float bandY = ToY(band->Threshold);
			drawList->AddRectFilled({ topLeft.x, bandY }, { topLeft.x + graphSize.x, prevY }, band->Color);

			if (band->Label && *band->Label) {
				const float textWidth = ImGui::CalcTextSize(band->Label).x;
				const float labelX = m_BandLabelAlign == BandLabelAlign::Right ? topLeft.x + graphSize.x - textWidth - 4 : topLeft.x + 4;

				drawList->AddText({ labelX, prevY - ImGui::GetTextLineHeight() - 2 }, IM_COL32(255, 255, 255, 100), band->Label);
			}

			prevY = bandY;
		}

		if (prevY > topLeft.y) {
			drawList->AddRectFilled({ topLeft.x, topLeft.y }, { topLeft.x + graphSize.x, prevY }, IM_COL32(100, 100, 100, 25));
		}
	}

	void StatisticGraph::DrawSideStats() const {
		ScopedGroup group;
		ImGui::SameLine();
		TextColored({ 0.3f, 1.f, 0.3f, 1.f }, "Min: {:.2f} {}", m_Min, m_Units);
		TextColored({ 0.3f, 0.7f, 1.0f, 1.f }, "Avg: {:.2f} {}", GetAverage(), m_Units);
		TextColored({ 1.0f, 0.3f, 0.3f, 1.f }, "Max: {:.2f} {}", m_Max, m_Units);
	}

	void StatisticGraph::DrawTargetLine(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const {
		//draw target line
		const float scaleRange = m_ScaleMax - m_ScaleMin;
		const float yRatio = 1.f - (m_Target - m_ScaleMin) / scaleRange;
		const float lineY = topLeft.y + graphSize.y * yRatio;

		const ImVec2 p1{ topLeft.x, lineY };
		const ImVec2 p2{ topLeft.x + graphSize.x, lineY };

		drawList->AddLine(p1, p2, IM_COL32(255, 0, 0, 90), 1.5f);
		drawList->AddText({ p1.x + 4, p1.y - ImGui::GetTextLineHeight() }, IM_COL32(255, 255, 255, 180), fmt::format("Target: {:.2f} {}", m_Target, m_Units).c_str());
	}
}
