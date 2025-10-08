#include "pch.h"
#include "Engine/Core/Application.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Threads/JobSystem.h"

#include "Engine/Layers/ImGuiLayer.h"
#include "Engine/Layers/StatisticLayer.h"
#include "Engine/Layers/LogLayer.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include "Engine/Devices/Mouse.h"
#include "Engine/Devices/MonitorRegistry.h"

#include <GLFW/glfw3.h>

#include "imgui.h"


namespace Engine {
	Application *Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &specs) : m_Specification(specs), m_TimeStepController(static_cast<uint32_t>(1000.f / 60.f), 60) {
		s_Instance = this;
	}

	Application::~Application() {
		MonitorRegistry::Get().Shutdown();

		Renderer::Shutdown();
		JobSystem::Shutdown();
	}

	void Application::OnEvent(Event &event) {
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		if(event.IsInCategory(EventCategoryKeyboard) && event.GetEventType() == KeyPressedEvent::GetStaticType()) {
			auto keyCode = dynamic_cast<KeyPressedEvent*>(&event)->GetKeyCode();

			if(keyCode == Key::Escape)
				Close();
		}

		for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if(event.Handled)
				break;

			(*it)->OnEvent(event);
		}
	}

	void Application::PushLayer(Ref<Layer> layer) {
		LOG_ENGINE_DEBUG("Adding {0} layer", layer->GetName());

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Ref<Layer> overlay) {
		LOG_ENGINE_DEBUG("Adding {0} overlay", overlay->GetName());

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Exit(int exitCode) {
		m_ExitCode = exitCode;
		m_Run      = false;
	}

	int Application::Run() {
		glfwSetTime(0);

		m_AppClock.Restart();
		m_TimeStepController.Restart();

		while(m_Run) {
			if(!m_Minimized) {
				m_TimeStepController.BeginFrame();
				Mouse::UpdateDelta();

				for(auto &layer : m_LayerStack) {
					layer->OnUpdate();
				}

				m_TimeStepController.BeginUpdates();
				while(m_TimeStepController.ShouldFixedUpdate()) {
					for(auto &layer : m_LayerStack)
						layer->OnConstUpdate(m_TimeStepController.GetScaledFixedDeltaTime());
				}

				m_ImGuiLayer->Begin();
				
				for(auto &layer : m_LayerStack)
					layer->OnImGuiRender();
				
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}

		return m_ExitCode;
	}

	void Application::ProcessArgs(const ApplicationCommandLineArgs &args) {
		LOG_ENGINE_TRACE("Processing command line args");

		m_CmdParser = MakeScope<CommandLineParser>(args);
		size_t i = 0;
		for(const auto arg : args) {
			LOG_ENGINE_TRACE("{}: {}", i++, arg);
		}

		if (m_CmdParser->Has("fullscreen"))
			m_Specification.FullScreen = true;

		if (m_CmdParser->Has("maximize"))
			m_Specification.FullWindow = true;

		const auto width = m_CmdParser->Get("width", "1027");
		const auto height = m_CmdParser->Get("height", "768");
		m_Specification.WindowSize = { std::stoul(width), std::stoul(height) };

		LOG_ENGINE_TRACE("Done parsing command line options");
	}

	void Application::Initialize() {
		auto logLayer = MakeRef<LogLayer>();
		m_Window = Window::Create(WindowProperties(m_Specification.Name, m_Specification.WindowSize));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		MonitorRegistry::Get().Initialize();
		MonitorRegistry::Get().SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		InitializeSettings();
		InitializeLua();

		LOG_ENGINE_INFO(
		                "Created window [Title: \"{}\", {:x}, Fullscreen: {}]",
		                m_Window->GetTitle(),
		                m_Window->GetSize(),
		                m_Window->IsFullscreen()
		               );

		LOG_ENGINE_INFO("Max updates: {}", m_TimeStepController.GetMaxUpdates());
		LOG_ENGINE_INFO("Update rate: {}", m_TimeStepController.GetUpdateRate());

		PushOverlay(m_ImGuiLayer = MakeRef<ImGuiLayer>());
		PushOverlay(MakeRef<StatisticLayer>());
		PushOverlay(logLayer);

		Renderer::Init();
		JobSystem::Init();
	}

	bool Application::OnWindowClose(const WindowCloseEvent &event) {
		Exit(0);
		return true;
	}

	bool Application::OnWindowResize(const WindowResizeEvent &event) {
		if(event.GetWidth() == 0 || event.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

		return false;
	}

	void Application::InitializeSettings() {
		if (m_Specification.FullScreen)
			m_Window->ToggleFullscreen();

		if (!m_Specification.FullScreen && m_Specification.FullWindow) {
			m_Window->Maximize();
		}
	}

	void Application::InitializeLua() {
		
	}
}
