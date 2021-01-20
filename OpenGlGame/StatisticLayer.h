#pragma once

#include "Types.h"

#include "Layer.h"
#include "Clock.h"

namespace Game
{
	class StatisticLayer : public Layer
	{
		bool m_Show = true;
		bool m_Processed = true;
		bool m_ShowMetric = true;

		uint64_t m_Updates = 0;
		uint64_t m_LastUpdates = 0;
		
		Clock m_Clock;

	public:
		StatisticLayer();

		void OnAttach() override;
		virtual void OnConstUpdate(const Time& ts) override;
		virtual void OnImGuiRender() override;

		virtual void OnUpdate() override;

		void IsVisible(bool visible) { m_Show = visible; }
		bool IsVisible() { return m_Show; }

	private:
		void ShortcutFunction();
	};
}