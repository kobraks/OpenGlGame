#pragma once

#include "Engine.h"

namespace Game
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate() override;

		void OnImGuiRender() override;
		void OnEvent(Event &event) override;

	private:

	};
}