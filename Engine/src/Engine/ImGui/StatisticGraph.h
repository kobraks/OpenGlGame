#pragma once
#include "Engine/Core/Base.h"
#include <array>
#include <string>
#include <string_view>

struct ImDrawList;
struct ImVec2;

namespace Engine {
	struct Band {
		float Threshold;
		uint32_t Color;
		const char* Label;
	};

	class StatisticGraph {
	public:
		explicit StatisticGraph(const std::string& name, uint64_t bufferSize = 256);

		void AddValue(float value);
		void Draw();
		void Draw(std::string_view label);
		void Reset();

		void SetTarget(float value) { m_Target = value; }
		void SetScaleMargin(float margin) { m_ScaleMargin = margin; }
		void SetUnit(const std::string& units) { m_Units = units; }

		void EnableDynamicScale(bool enable = true) { m_DynamicScale = enable; }
		bool IsDynamicScale() const { return m_DynamicScale; }

		void TargetVisible(bool enable = true) { m_TargetVisible = enable; }
		bool IsTargetVisible() const { return m_TargetVisible; }

		void EnableSideStats(bool enable = true) { m_ShowSideStats = enable; }
		bool AreSideStatsVisible() const { return m_ShowSideStats; }

		void SetScale(float minValue, float maxValue) { m_ScaleMin = minValue; m_ScaleMax = maxValue; }

		void AddBand(float threshold, uint32_t color, const char* label);
		const Band& GetBand(size_t index) const;
		void ModifyBand(size_t index, float newThreshold, uint32_t newColor, const char* newLabel);
		size_t GetBandCount() const { return m_Bands.size(); }
		void RemoveBand(size_t index);
		void ClearBands();

		float GetAverage() const;
		float GetMin() const { return m_Min; }
		float GetMax() const { return m_Max; }

		uint64_t GetBufferSize() const { return m_BufferSize; }
	protected:
		void RecalculateMinMax();
		void RebuildOrderedBuffer();

		const std::vector<const Band*>& GetSortedBands() const;

		void DrawGraph() const;
		void DrawValueBands(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const;
		void DrawSideStats() const;
		void DrawTargetLine(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const;

	private:
		std::string m_Name;
		std::string m_Units;

		std::vector<float> m_History;
		std::vector<float> m_PlotBuffer;
		std::vector<Band> m_Bands;

		uint64_t m_BufferSize;
		uint64_t m_HeadIndex = 0;
		uint64_t m_Count = 0;

		float m_Total = 0.0f;
		float m_Min = 0.0f;
		float m_Max = 0.0f;
		float m_Target = 0.0f;

		bool m_TargetVisible = true;
		bool m_ShowSideStats = true;

		bool m_DynamicScale = false;
		float m_ScaleMargin = 10.0f;
		float m_ScaleMin = 0.0f;
		float m_ScaleMax = 50.0f;

		bool m_HistoryDirty = true;

		mutable std::vector<const Band*> m_SortedBandsCache;
		mutable bool m_BandSortedDirty = true;
	};
}
