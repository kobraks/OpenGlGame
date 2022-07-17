#include "pch.h"
#include "Application.h"

#include "Log.h"

#include "Engine/Lua/LuaRegister.h"

#include "Engine/Layers/ImGuiLayer.h"
#include "Engine/Layers/StatisticLayer.h"
#include "Engine/Layers/LogLayer.h"
#include "Engine/Layers/ConsoleLayer.h"
#include "Engine/Layers/ConfigLayer.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Devices/Mouse.h"
#include "Engine/Lua/LuaUtils.h"

#include "Engine/Events/ApplicationEvent.h"

#include <GLFW/glfw3.h>
#include <lua.hpp>

// #include "Renderer.h"

#include "ImGui.h"
// #include "TextureLoader.h"

namespace
{
	constexpr spdlog::level::level_enum GetSeverity(GLenum severity)
	{
		switch(severity)
		{
			case GL_DEBUG_SEVERITY_HIGH: return spdlog::level::critical;
			case GL_DEBUG_SEVERITY_MEDIUM: return spdlog::level::err;
			case GL_DEBUG_SEVERITY_LOW: return spdlog::level::warn;
			case GL_DEBUG_SEVERITY_NOTIFICATION: return spdlog::level::info;
			default: return spdlog::level::trace;
		}
	}

	constexpr std::string_view GetSource(GLenum source)
	{
		switch(source)
		{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_APPLICATION: return "Application";
			case GL_DEBUG_SOURCE_OTHER: return "Other";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
			default: return "Unknown";
		}
	}

	constexpr std::string_view GetType(GLenum type)
	{
		switch(type)
		{
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
			case GL_DEBUG_TYPE_ERROR: return "Error";
			case GL_DEBUG_TYPE_MARKER: return "Maker";
			case GL_DEBUG_TYPE_OTHER: return "Other";
			case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
			case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
			case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
			case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
			default: return "Unknown";
		}
	}

	void EnableInputMode(int inputMode)
	{
		if(inputMode == static_cast<int>(Game::InputMode::LockKeyModes) || inputMode == static_cast<int>(Game::InputMode::RawMouseMotion) || inputMode ==
			static_cast<int>(Game::InputMode::StickyKeys) || inputMode == static_cast<int>(Game::InputMode::StickyMouseButtons))
			Game::Application::Get().GetWindow().SetInputMode(true, static_cast<Game::InputMode>(inputMode));
		else
			SCRIPT_LOG_ERROR(
		                 "Unknown input mode: {}, Expected: {}, {}, {} or {}",
		                 inputMode,
		                 static_cast<int>(Game::InputMode::StickyKeys),
		                 static_cast<int>(Game::InputMode::StickyMouseButtons),
		                 static_cast<int>(Game::InputMode::LockKeyModes),
		                 static_cast<int>(Game::InputMode::RawMouseMotion)
		                );
	}

	void DisableInputMode(int inputMode)
	{
		if(inputMode == static_cast<int>(Game::InputMode::LockKeyModes) || inputMode == static_cast<int>(Game::InputMode::RawMouseMotion) || inputMode ==
			static_cast<int>(Game::InputMode::StickyKeys) || inputMode == static_cast<int>(Game::InputMode::StickyMouseButtons))
			Game::Application::Get().GetWindow().SetInputMode(false, static_cast<Game::InputMode>(inputMode));
		else
			SCRIPT_LOG_ERROR(
		                 "Unknown input mode: {}, Expected: {}, {}, {} or {}",
		                 inputMode,
		                 static_cast<int>(Game::InputMode::StickyKeys),
		                 static_cast<int>(Game::InputMode::StickyMouseButtons),
		                 static_cast<int>(Game::InputMode::LockKeyModes),
		                 static_cast<int>(Game::InputMode::RawMouseMotion)
		                );
	}

	bool GetInputMode(int inputMode)
	{
		if(inputMode == static_cast<int>(Game::InputMode::LockKeyModes) || inputMode == static_cast<int>(Game::InputMode::RawMouseMotion) || inputMode ==
			static_cast<int>(Game::InputMode::StickyKeys) || inputMode == static_cast<int>(Game::InputMode::StickyMouseButtons))
			Game::Application::Get().GetWindow().GetInputMode(static_cast<Game::InputMode>(inputMode));
		else
			SCRIPT_LOG_ERROR(
		                 "Unknown input mode: {}, Expected: {}, {}, {} or {}",
		                 inputMode,
		                 static_cast<int>(Game::InputMode::StickyKeys),
		                 static_cast<int>(Game::InputMode::StickyMouseButtons),
		                 static_cast<int>(Game::InputMode::LockKeyModes),
		                 static_cast<int>(Game::InputMode::RawMouseMotion)
		                );

		return false;
	}

	void SetCursorMode(int cursorMode)
	{
		if(cursorMode == static_cast<int>(Game::CursorMode::Normal) || cursorMode == static_cast<int>(Game::CursorMode::Hidden) || cursorMode == static_cast<
			int>(Game::CursorMode::Disabled))
			Game::Application::Get().GetWindow().SetCursorMode(static_cast<Game::CursorMode>(cursorMode));
		else
			SCRIPT_LOG_ERROR(
		                 "Unknown Cursor mode: {}, Expected: {}, {}, {}",
		                 cursorMode,
		                 static_cast<int>(Game::CursorMode::Normal),
		                 static_cast<int>(Game::CursorMode::Hidden),
		                 static_cast<int>(Game::CursorMode::Disabled)
		                );
	}

	int GetCursorMode()
	{
		return static_cast<int>(Game::Application::Get().GetWindow().GetCursorMode());
	}

	void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		Game::Log::GetOpenGLLogger()->log(GetSeverity(severity), "ID: {}, Source: {}, Type: {}, Message = {}", id, GetSource(source), GetType(type), message);
	}
}

namespace Game
{
	bool s_ShowImGuiTest = false;

	Application *Application::s_Instance = nullptr;

	// Application::Application()
	// {
	// 	 Log::Init();
	// }

	Application::Application(const ApplicationSpecification &specification)
	{
		s_Instance = this;
	}

	Application::~Application()
	{
		// TextureLoader::ClearCashed();
	}

	void Application::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		if(event.IsInCategory(EventCategoryKeyboard) && event.GetEventType() == KeyPressedEvent::GetStaticType())
		{
			auto keyCode = dynamic_cast<KeyPressedEvent*>(&event)->GetKeyCode();

			if(keyCode == Key::Escape)
				Close();
			if(keyCode == Key::F1)
				s_ShowImGuiTest = !s_ShowImGuiTest;
		}

		for(auto &shortcut : m_Shortcuts)
			shortcut.OnEvent(event);

		for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if(event.Handled)
				break;

			(*it)->OnEvent(event);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		LOG_DEBUG("Adding {0} layer", layer->GetName());
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		LOG_DEBUG("Adding {0} overlay", overlay->GetName());
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::RegisterShortcut(const Shortcut &shortcut)
	{
		m_Shortcuts.emplace_back(shortcut);
	}

	void Application::Exit(int exitCode)
	{
		m_ExitCode = exitCode;
		m_Running  = false;
	}

	int Application::Run()
	{
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

				for(Layer* layer : m_LayerStack)
				{
					layer->OnUpdate();
				}

				uint64_t updates = 0;

				int32_t updateTime = updateClock.GetElapsedTime().AsMilliseconds();
				while((updateTime - updateNext) >= m_UpdateRate && updates++ < m_MaxUpdates)
				{
					for(Layer* layer : m_LayerStack)
						layer->OnConstUpdate(Milliseconds(m_UpdateRate));
					updateNext += m_UpdateRate;
				}

				m_ImGuiLayer->Begin();

				if(s_ShowImGuiTest)
					ImGui::ShowDemoWindow(&s_ShowImGuiTest);

				for(Layer* layer : m_LayerStack)
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

	void Application::SetUpdateRate(float rate)
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
		Log::GetOpenGLLogger()->set_level(spdlog::level::debug);

		m_Properties = MakeScope<PropertyManager>();

		InitializeLua();
		InitializeSettings();

		m_Window = std::make_unique<Window>(WindowProperties{"Game", 800, 600});
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		LOG_INFO("Created Window [With: {}, Height: {}, Name: \"{}\"]", m_Window->GetWidth(), m_Window->GetHeight(), m_Window->GetTitle());
		LOG_INFO("Max updates: {0}", m_MaxUpdates);
		LOG_INFO("Update rate {0}", m_UpdateRate);

		LOG_INFO("Creating thread pool with {} threads", std::thread::hardware_concurrency());
		// m_ThreadPool = MakeScope<ThreadPool>(std::thread::hardware_concurrency());

		PushOverlay(m_ImGuiLayer = new ImGuiLayer());
		PushOverlay(&(*logLayer));
		PushOverlay(new StatisticLayer());
		PushOverlay(new ConsoleLayer());
		// PushOverlay(MakePointer<ConfigLayer>());

		auto OpenGL = m_Window->GetFunctions();

		// OpenGL.Enable(Capability::Blend);
		// OpenGL.Enable(Capability::CullFace);
		OpenGL.Enable(Capability::DepthTest);

#ifdef _DEBUG
		OpenGL.Enable(Capability::DebugOutput);
		OpenGL.Enable(Capability::DebugOutputSynchronous);

		OpenGL.SetDebugMessageCallback(DebugCallback, nullptr);
#endif
	}

#define ENUM_TO_STRING_ENUM(e, v) #v,  static_cast<int>(e::##v)
#define BIND_FUNCTION(f) #f, f

	void Application::InitializeLua()
	{
		LOG_INFO("Starting Lua machine");
		m_Lua     = MakeScope<sol::state>();
		auto &lua = *m_Lua;

		LOG_DEBUG("Opening Lua libraries");
		lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::coroutine, sol::lib::debug, sol::lib::math, sol::lib::os, sol::lib::io);

		LOG_TRACE("Registering Application arguments");
		lua["ArgumentCount"] = m_Arguments.size();
		lua.create_named_table("Arguments");

		for(size_t i                = 0; i < m_Arguments.size(); ++i)
			lua["Arguments"][i + 1] = m_Arguments[0];

		LOG_TRACE("Registering Mouse and Keyboard");
		Mouse::RegisterLua(lua);
		Keyboard::RegisterLua(lua);
		LOG_TRACE("Registering Input modes");

		auto inputModeTable = lua.create_table_with();
		inputModeTable.set(ENUM_TO_STRING_ENUM(InputMode, LockKeyModes));
		inputModeTable.set(ENUM_TO_STRING_ENUM(InputMode, RawMouseMotion));
		inputModeTable.set(ENUM_TO_STRING_ENUM(InputMode, StickyKeys));
		inputModeTable.set(ENUM_TO_STRING_ENUM(InputMode, StickyMouseButtons));

		auto cursorModeTable = lua.create_table_with();
		cursorModeTable.set(ENUM_TO_STRING_ENUM(CursorMode, Normal));
		cursorModeTable.set(ENUM_TO_STRING_ENUM(CursorMode, Hidden));
		cursorModeTable.set(ENUM_TO_STRING_ENUM(CursorMode, Disabled));


		auto inputMetaTable = lua.create_table_with();
		inputMetaTable.set_function(BIND_FUNCTION(SetCursorMode));
		inputMetaTable.set_function(BIND_FUNCTION(GetCursorMode));
		inputMetaTable.set_function("EnableMode", EnableInputMode);
		inputMetaTable.set_function("DisableMode", DisableInputMode);
		inputMetaTable.set_function("GetInputMode", GetInputMode);
		inputMetaTable.set_function("GetInputMode", GetInputMode);
		inputMetaTable.set_function("IsRawMouseMotionSupported", [this]() { return m_Window->IsRawMouseInputSupported(); });
		inputMetaTable.set("Keyboard", lua["Keyboard"]);
		inputMetaTable.set("Keyboard", lua["Mouse"]);
		inputMetaTable.set("Mode", inputModeTable);
		inputMetaTable.set("CursorMode", cursorModeTable);

		SetAsReadOnlyTable(inputMetaTable["Mode"], inputModeTable, Deny);
		SetAsReadOnlyTable(inputMetaTable["CursorMode"], cursorModeTable, Deny);

		auto inputTable = lua.create_named_table("Input");

		SetAsReadOnlyTable(inputTable, inputMetaTable, Deny);

		LOG_TRACE("Registering Application");
		LuaRegister(*m_Properties);
		m_Lua->set("Properties", *m_Properties);

		auto applicationMetaTable          = lua.create_table_with();
		applicationMetaTable["GetTime"]    = [this]() { return m_Clock.GetElapsedTime().AsSeconds(); };
		applicationMetaTable["Properties"] = lua["Properties"];
		applicationMetaTable["Exit"]       = [this](sol::variadic_args args)
		{
			if(args.size() == 1)
			{
				if(args[0].is<int>())
					Exit(args[0].as<int>());
				else
					SCRIPT_LOG_ERROR("Unknown parameter type");
			}
			else if(args.size() == 0)
				Exit(0);
			else
				SCRIPT_LOG_ERROR("Called with too big amount of arguments");
		};

		auto applicationTable = lua.create_named_table("Application");
		SetAsReadOnlyTable(applicationTable, applicationMetaTable, Deny);

		lua["Properties"] = sol::nil;
		lua["Exit"]       = lua["Application"]["Exit"];
	}

	void Application::InitializeSettings()
	{
		m_Properties->Add<float>("UpdateRate", [this]()->float { return this->GetUpdateRate(); }, [this](float value) { this->SetUpdateRate(value); });
		m_Properties->Add<uint64_t>("MaxUpdates", [this]()->uint64_t { return this->GetMaxUpdates(); }, [this](uint64_t value) { this->SetMaxUpdates(value); });
		m_Properties->Add<uint32_t>(
		                            "WindowWidth",
		                            [this]() { return this->GetWindow().GetSize().Width; },
		                            [this](uint32_t value)
		                            {
			                            this->GetWindow().SetSize(value, this->GetWindow().GetHeight());
		                            }
		                           );
		m_Properties->Add<uint32_t>(
		                            "WindowHeight",
		                            [this]() { return this->GetWindow().GetWidth(); },
		                            [this](uint32_t value)
		                            {
			                            this->GetWindow().SetSize(this->GetWindow().GetWidth(), value);
		                            }
		                           );
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
