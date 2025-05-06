#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
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

		void TargetVisible(bool enable = true)

		void SetScale(float minValue, float maxValue) { m_Scale = Vector2f{ minValue, maxValue }; }

		void AddBand(float threshold, uint32_t color, const char* label);
		const Band& GetBand(size_t index) const;
		Band& GetBand(size_t index);
		size_t GetBandCount() const { return m_Bands.size(); }
		void ClearBands();

		float GetAverage() const;
		float GetMin() const { return m_Min; }
		float GetMax() const { return m_Max; }

	protected:
		void RecalculateMinMax();
		void RebuildOrderedBuffer();
		void DrawGraph() const;
		void DrawBands(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const;
		void DrawSideStats() const;
		void DrawTargetLine(const ImVec2& topLeft, const ImVec2& graphSize, ImDrawList* drawList) const;

		std::string m_Name;
		std::string m_Units;

		std::vector<float> m_History;
		std::vector<float> m_OrderedHistory;
		std::vector<Band> m_Bands;

		uint64_t m_BufferSize;
		uint64_t m_HeadIndex = 0;
		uint64_t m_Count = 0;

		float m_Total = 0.0f;
		float m_Min = 0.0f;
		float m_Max = 0.0f;
		float m_Target = 0.0f;

		bool m_TargetVisible = true;

		bool m_DynamicScale = false;
		float m_ScaleMargin = 10.0f;
		Vector2f m_Scale{ 0.0f, 50.0f };

		bool m_Dirty = true;
	};
}
