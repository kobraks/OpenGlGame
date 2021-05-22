#include "pch.h"
#include "GameLayer.h"

#include "Defines.h"
#include "Application.h"
#include "Renderer.h"
#include "SceneSerializer.h"

namespace Game
{
	GameLayer::GameLayer() : Layer("GameLayer") {}

	void GameLayer::OnAttach()
	{
		m_Scene = MakePointer<Scene>();
		SceneSerializer serializer(m_Scene);

		if(serializer.Deserialize(SCENE_PATH "/MainScene.lua"))
			LOG_INFO("Sucessfuly imported scene");
		else
			LOG_ERROR("Unable to deserialize scene");

		m_MainShader = ShaderLoader::Load("Shader", SHADERS_PATH);

		Application::Get().GetWindow().SetVSync(false);
		Log::GetOpenGLLogger()->set_level(spdlog::level::err);
	}

	void GameLayer::OnDetach() { }

	void GameLayer::OnUpdate()
	{
		m_Scene->OnUpdate();
		
		// Renderer::SetShader(m_MainShader);
		// m_Scene->OnDraw();
	}

	void GameLayer::OnConstUpdate(const Time &timeStep)
	{
		m_Scene->OnConstUpdate(timeStep);
	}

	void GameLayer::OnImGuiRender() { }

	void GameLayer::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(GameLayer::ResizeEvent));
	}

	bool GameLayer::ResizeEvent(WindowResizeEvent &event)
	{
		m_Scene->OnViewportResize(event.GetWidth(), event.GetHeight());
		return false;
	}
}
