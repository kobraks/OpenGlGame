#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

#include "Engine/Core/Clock.h"

#include "Engine/Events/KeyEvent.h"
#include "Engine/ImGui/StatisticGraph.h"
#include "Engine/ImGui/StatisticGraphGroup.h"

namespace Engine {
	class StatisticLayer : public Layer {
	public:
		StatisticLayer();

		void OnAttach() override;
		void OnUpdate() override;
		void OnConstUpdate(const Time &timeStep) override;
		void OnImGuiRender() override;

		void OnEvent(Event& event) override;

		bool OnKeyPressEvent(const KeyPressedEvent& event);

		void Visible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }
	private:

		bool m_Show = false;
		bool m_Processed = true;
		bool m_ShowMetric = true;

		uint64_t m_ConstUpdateCount = 0;
		uint64_t m_LastConstUpdateCount = 0;
		uint64_t m_InstantUpdateCount = 0;

		Ref<StatisticGraph> m_FrameGraph;
		Ref<StatisticGraph> m_UpdateGraph;
		StatisticGraphGroup m_GraphGroup;

		Clock m_Clock;
	};
}
