#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

namespace Engine {
	class ImGuiLayer: public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer() override = default;

		void OnAttach() final;
		void OnDetach() final;
		void OnEvent(Event &e) final;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeMode();

	private:
		bool m_BlockEvents = true;
	};
}
