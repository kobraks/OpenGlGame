#include "pch.h"
#include "GameLayer.h"

#include "Application.h"

namespace Game
{
	GameLayer::GameLayer() : Layer("GameLayer") {}

	void GameLayer::OnAttach()
	{
	}

	void GameLayer::OnDetach()
	{
	}

	void GameLayer::OnUpdate()
	{
	}

	void GameLayer::OnConstUpdate(const Time &timeStep) {}

	void GameLayer::OnImGuiRender()
	{
	}

	void GameLayer::OnEvent(Event &event) {}
}
