#include "pch.h"
#include "Application.h"

#include "Log.h"
#include "LuaRegister.h"

#include "ImGuiLayer.h"
#include "StatisticLayer.h"
#include "ConsoleLayer.h"
#include "LogLayer.h"
#include "Renderer.h"

#include "ImGui.h"

namespace
{
	constexpr spdlog::level::level_enum GetSeverity(GLenum severity)
	{
		switch(severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			return spdlog::level::critical;
		case GL_DEBUG_SEVERITY_MEDIUM:
			return spdlog::level::err;
		case GL_DEBUG_SEVERITY_LOW:
			return spdlog::level::warn;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return spdlog::level::info;
		default:
			return spdlog::level::trace;
		}
	}

	constexpr std::string_view GetSource(GLenum source)
	{
		switch(source)
		{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "Application";
		case GL_DEBUG_SOURCE_OTHER:
			return "Other";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "Shader Compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "Third Party";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "Window System";
		default:
			return "Unknown";

		}
	}

	constexpr std::string_view GetType(GLenum type)
	{
		switch(type)
		{
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "Deprecated Behavior";
		case GL_DEBUG_TYPE_ERROR:
			return "Error";
		case GL_DEBUG_TYPE_MARKER:
			return "Maker";
		case GL_DEBUG_TYPE_OTHER:
			return "Other";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "Performance";
		case GL_DEBUG_TYPE_POP_GROUP:
			return "Pop Group";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "Portability";
		case GL_DEBUG_TYPE_PUSH_GROUP:
			return "Push Group";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "Undefined Behavior";
		default:
			return "Unknown";
		}
	}
	
	void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam)
	{
		Game::Log::GetOpenGLLogger()->log(GetSeverity(severity), "ID: {}, Source: {}, Type: {}, Message = {}", id, GetSource(source), GetType(type), message);
	}
}

namespace Game
{
	bool s_ShowImGuiTest = false;

	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		s_Instance = this;
	}

	Application::~Application() { }

	void Application::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		if(event.IsInCategory(EventCategoryKeyboard) && event.GetEventType() == KeyPressedEvent::GetStaticType())
		{
			auto keyCode = dynamic_cast<KeyPressedEvent*>(&event)->GetKeyCode();

			if(keyCode == Key::Escape) Close();
			if(keyCode == Key::F1) s_ShowImGuiTest = !s_ShowImGuiTest;
		}

		for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if(event.Handled) break;

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

		m_Clock.Restart();

		Clock clock;
		Clock updateClock;
		uint32_t updateNext = updateClock.GetElapsedTime().AsMilliseconds(); 
		
		while(m_Running)
		{
			m_Window->GetFunctions().Clear(BufferBit::Color | BufferBit::Depth);
			if(!m_Minimalized)
			{
				m_FrameTime = clock.Restart();

				for(Pointer<Layer> &layer : m_LayerStack)
				{
					layer->OnUpdate();
				}

				uint64_t updates = 0;

				int32_t updateTime = updateClock.GetElapsedTime().AsMilliseconds();
				while((updateTime - updateNext) >= m_UpdateRate && updates++ < m_MaxUpdates)
				{
					for(Pointer<Layer> &layer : m_LayerStack)
						layer->OnConstUpdate(Milliseconds(m_UpdateRate));
					updateNext += m_UpdateRate;
				}

				m_ImGuiLayer->Begin();

				if(s_ShowImGuiTest) ImGui::ShowDemoWindow(&s_ShowImGuiTest);

				for(Pointer<Layer> &layer : m_LayerStack) layer->OnImGuiRender();

				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}

		return m_ExitCode;
	}

	void Application::ProcessArgs(int argc, char **argv)
	{
		for(int i = 0; i < argc; ++i) m_Arguments.emplace_back(argv[i]);
	}

	void Application::SetUpdateRate(uint32_t rate)
	{
		if(200 >= rate && 1 <= rate)
		{
			m_UpdateRate = static_cast<uint32_t>(1000.f / rate);
		}
	}

	void Application::SetMaxUpdates(uint64_t maxUpdates)
	{
		if(200 >= maxUpdates && 1 <= maxUpdates)
		{
			m_MaxUpdates = maxUpdates;
		}
	}

	void Application::LuaRegister(Game::LuaRegister &luaObject)
	{
		luaObject.Register(*m_Lua);
	}

	void Application::Initialize()
	{
		auto logLayer = MakePointer<LogLayer>();

		Log::GetScriptLogger()->sinks().push_back(logLayer);
		Log::GetOpenGLLogger()->sinks().push_back(logLayer);
		Log::GetApplicationLogger()->sinks().push_back(logLayer);
		Log::GetAssertionLogger()->sinks().push_back(logLayer);

		LOG_INFO("Starting Lua machine");
		m_Lua = MakeScope<sol::state>();
		m_Lua->open_libraries(sol::lib::base);

		(*m_Lua)["ArgumentCount"] = m_Arguments.size();
		m_Lua->create_named_table("Arguments");

		for(size_t i = 0; i < m_Arguments.size(); ++i) (*m_Lua)["Arguments"][i + 1] = m_Arguments[0];

		m_Window = std::make_unique<Window>(WindowProperties{"Game", 800, 600});
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		LOG_INFO(
		         "Created Window [With: {}, Height: {}, Name: \"{}\"]",
		         m_Window->GetWidth(),
		         m_Window->GetHeight(),
		         m_Window->GetTitle()
		        );
		LOG_INFO("Max updates: {0}", m_MaxUpdates);
		LOG_INFO("Update rate {0}", m_UpdateRate);

		m_ImGuiLayer = MakePointer<ImGuiLayer>();

		PushOverlay(m_ImGuiLayer);
		PushOverlay(logLayer);
		PushOverlay(MakePointer<StatisticLayer>());
		PushOverlay(MakePointer<ConsoleLayer>());

		auto OpenGL = m_Window->GetFunctions();

		OpenGL.Enable(Capability::Blend);
		OpenGL.Enable(Capability::CullFace);
		OpenGL.Enable(Capability::DepthTest);
		OpenGL.Enable(Capability::DebugOutput);
		OpenGL.SetDebugMessageCallback(DebugCallback, nullptr);
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
