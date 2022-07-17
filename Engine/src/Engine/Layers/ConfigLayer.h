#pragma once

#include "Engine/Layers/Layer.h"

namespace Game
{
	class ConfigLayer : public Layer
	{
		bool m_Show = true;
		
	public:
		ConfigLayer();

		void OnEvent(Event &event) override;
		void OnAttach() override;
		void OnImGuiRender() override;

	private:
	};
}