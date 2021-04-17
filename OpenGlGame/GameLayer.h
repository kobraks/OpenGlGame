#pragma once
#include "Layer.h"

#include "Scene.h"
#include "ApplicationEvent.h"

namespace Game
{
	class GameLayer : public Layer
	{
		Pointer<Scene> m_Scene;
		
	public:
		GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnConstUpdate(const Time &timeStep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event &event) override;

	private:
		bool ResizeEvent(WindowResizeEvent &event);
	};
}