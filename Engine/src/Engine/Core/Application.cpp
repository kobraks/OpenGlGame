#include "pch.h"
#include "Engine/Core/Application.h"

#include "Engine/Layers/ImGuiLayer.h"
#include "Engine/Layers/StatisticLayer.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

#include "Engine/Events/KeyEvent.h"

namespace Engine {
	Application *Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &specs) : m_Specification(specs) {
		s_Instance = this;
	}

	Application::~Application() {}

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

		m_Clock.Restart();

		Clock clock;
		Clock updateClock;
		uint32_t updateNext = updateClock.GetElapsedTime().AsMilliseconds();

		while(m_Run) {
			//TODO clear

			if(!m_Minimalized) {
				m_FrameTime = clock.Restart();

				for(auto &layer : m_LayerStack) {
					layer->OnUpdate();
				}

				uint64_t updates = 0;

				int32_t updateTime = updateClock.GetElapsedTime().AsMilliseconds();
				while((updateTime - updateNext) >= m_UpdateRate && updates++ < m_MaxUpdates) {
					for(auto &layer : m_LayerStack)
						layer->OnConstUpdate(Milliseconds(m_UpdateRate));

					updateNext += m_UpdateRate;
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
		for(const auto arg : args)
			m_Arguments.emplace_back(arg);
	}

	void Application::Initialize() {
		m_Window = Window::Create(WindowProperties(m_Specification.Name, m_Specification.WindowSize));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		InitializeSettings();
		InitializeLua();

		LOG_ENGINE_INFO(
		                "Created window [Title: \"{}\", Widtth: {}, Height: {}, Fullscreen: {}]",
		                m_Window->GetTitle(),
		                m_Window->GetWidth(),
		                m_Window->GetHeight(),
		                m_Window->IsFullscreen()
		               );

		LOG_ENGINE_INFO("Max updates: {}", m_MaxUpdates);
		LOG_ENGINE_INFO("Update rate: {}", m_UpdateRate);

		PushOverlay(m_ImGuiLayer = MakeRef<ImGuiLayer>());
		PushOverlay(MakeRef<StatisticLayer>());
	}

	bool Application::OnWindowClose(const WindowCloseEvent &event) {
		Exit(0);
		return true;
	}

	bool Application::OnWindowResize(const WindowResizeEvent &event) {
		if(event.GetWidth() == 0 || event.GetHeight() == 0) {
			m_Minimalized = true;
			return false;
		}

		m_Minimalized = false;
		return false;
	}

	void Application::InitializeSettings() {
		
	}

	void Application::InitializeLua() {
		
	}
}
