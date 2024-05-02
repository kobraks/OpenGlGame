#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

#include "Engine/Core/Clock.h"

namespace Engine {
	class StatisticLayer : public Layer {
	public:
		StatisticLayer();

		void OnAttach() override;
		void OnConstUpdate(const Time &timeStep) override;
		void OnImGuiRender() override;

		void OnUpdate() override;

		void Visible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }
	private:
		bool m_Show = true;
		bool m_Processed = true;
		bool m_ShowMetric = true;

		uint64_t m_Updates = 0;
		uint64_t m_LastUpdates = 0;

		Clock m_Clock;
	};
}