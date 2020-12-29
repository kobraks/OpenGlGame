#include "pch.h"
#include "Application.h"

#include "Log.h"
#include "LuaRegister.h"

#include "ImGuiLayer.h"
#include "StatisticLayer.h"
#include "LogLayer.h"

namespace Game
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		s_Instance = this;
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if(event.Handled)
				break;

			(*it)->OnEvent(event);
		}
	}

	void Application::PushLayer(Pointer<Layer> layer)
	{
		LOG_DEBUG("Adding {0} layer", layer->GetName());
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Pointer<Layer> overlay)
	{
		LOG_DEBUG("Adding {0} overlay", overlay->GetName());
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Exit(int exitCode)
	{
		m_ExitCode = exitCode;
		m_Running  = false;
	}

	int Application::Run()
	{
		Initialize();
		glfwSetTime(0.);

		auto time = Time::Zero;

		m_Clock.Restart();

		Clock clock;
		Clock updateClock;

		while(m_Running)
		{
			if(!m_Minimalized)
			{
				m_FrameTime = clock.Restart();

				for(Pointer<Layer>& layer : m_LayerStack)
				{
					layer->OnUpdate();
				}

				uint64_t updates = 0;
				bool update      = false;
				auto nextTime    = Time::Zero;

				time = updateClock.GetElapsedTime();
				while((time - nextTime) >= m_UpdateRate && updates++ < m_MaxUpdates)
				{
					for(Pointer<Layer>& layer : m_LayerStack)
					{
						layer->OnConstUpdate(m_UpdateRate);
						nextTime += m_UpdateRate;
						update = true;
					}
				}

				if(update)
					updateClock.Restart();

				m_ImGuiLayer->Begin();

				for(Pointer<Layer>& layer : m_LayerStack)
					layer->OnImGuiRender();

				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}

		return m_ExitCode;
	}

	void Application::ProcessArgs(int argc, char **argv)
	{
		for(int i = 0; i < argc; ++i)
			m_Arguments.emplace_back(argv[i]);
	}

	void Application::SetUpdateRate(float frequency)
	{
		if(200.f >= frequency && 1. <= frequency)
		{
			m_UpdateRate = Seconds(1.f / frequency);
			m_Frequency  = frequency;
		}
	}

	void Application::SetMaxUpdates(uint64_t maxUpdates)
	{
		if(200 >= maxUpdates && 1 <= maxUpdates)
		{
			m_MaxUpdates = maxUpdates;
		}
	}

	void Application::LuaRegister(Game::LuaRegister &luaObject) { }

	void Application::Initialize()
	{
		auto logLayer = MakePointer<LogLayer>();

		Log::GetScriptLogger()->sinks().push_back(logLayer);
		Log::GetOpenGLLogger()->sinks().push_back(logLayer);
		Log::GetApplicationLogger()->sinks().push_back(logLayer);
		Log::GetAssertionLogger()->sinks().push_back(logLayer);
		
		m_Window = std::make_unique<Window>(WindowProperties{"Game", 800, 600});
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = MakePointer<ImGuiLayer>();

		PushOverlay(m_ImGuiLayer);
		PushOverlay(MakePointer<StatisticLayer>());
		PushOverlay(logLayer);

		LOG_INFO("Max updates: {0}", m_MaxUpdates);
		LOG_INFO("Update frequency {0}", m_Frequency);
	}

	bool Application::OnWindowClose(WindowCloseEvent &event)
	{
		Exit(0);
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent &event)
	{
		if(event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			m_Minimalized = true;
			return false;
		}

		m_Minimalized = false;


		return false;
	}
}
