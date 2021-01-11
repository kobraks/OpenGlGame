#pragma once

#include "Layer.h"
#include "Clock.h"

namespace Game
{
	class StatisticLayer : public Layer
	{
		class Metric
		{
			constexpr static inline size_t s_HISTORY_SAMPLES = 256;
			
			float m_Min = 0;
			float m_Max = 0;

			double m_Total = 0;

			bool m_First = true;

			float m_History[s_HISTORY_SAMPLES];
			size_t m_HistoryCount = 0;
		
		public:
			Metric();

			void operator()(float time);
			void Draw() const;

			float Min() const { return m_Min; }
			float Max() const { return m_Max; }
			float Avr() const { return m_Total / m_HistoryCount; }
		};
		
		bool m_Show = true;
		bool m_Processed = true;
		bool m_ShowMetric = true;

		uint64_t m_Updates = 0;
		uint64_t m_LastUpdates = 0;
		
		Clock m_Clock;

		Metric m_FrameMetric;
		Metric m_UpdateMetric;
	public:
		StatisticLayer();

		virtual void OnConstUpdate(const Time& ts) override;
		virtual void OnImGuiRender() override;

		virtual void OnUpdate() override;

		void IsVisible(bool visible) { m_Show = visible; }
		bool IsVisible() { return m_Show; }
	};
}