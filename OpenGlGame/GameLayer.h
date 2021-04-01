#pragma once
#include "Layer.h"

namespace Game
{
	class GameLayer : public Layer
	{
	public:
		GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnConstUpdate(const Time &timeStep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event &event) override;
	};
}