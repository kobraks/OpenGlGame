#pragma once

#include <vector>

#include "Types.h"

#include "Window.h"
#include "LayerStack.h"
#include "Event.h"
#include "ApplicationEvent.h"

#include "Time.h"
#include "Clock.h"

#include "Shortcut.h"

namespace Game
{
	class LuaRegister;

	class ImGuiLayer;

	class Application
	{
		Scope<Window> m_Window;
		Scope<sol::state> m_Lua;

		bool m_Running     = true;
		bool m_Minimalized = false;

		LayerStack m_LayerStack;

		Pointer<ImGuiLayer> m_ImGuiLayer;

		int m_ExitCode = 0;

		uint64_t m_MaxUpdates = 60;
		uint32_t m_UpdateRate = static_cast<uint32_t>(1000.f / 60.f);

		Time m_FrameTime  = Time::Zero;

		Clock m_Clock;

		std::vector<std::string> m_Arguments;
		
		std::vector<Shortcut> m_Shortcuts;

		static Application *s_Instance;

	public:
		Application();
		~Application();

		void OnEvent(Event &event);
		void PushLayer(Pointer<Layer> layer);
		void PushOverlay(Pointer<Layer> overlay);

		void RegisterShortcut(const Shortcut& shortcut);

		Window& GetWindow() const { return *m_Window; }

		void Close() { Exit(0); }
		void Exit(int exitCode);

		static Application& Get() { return *s_Instance; }
		int Run();

		void ProcessArgs(int argc, char **argv);

		void SetUpdateRate(uint32_t rate);
		float GetUpdateRate() const { return 1000.f / static_cast<float>(m_UpdateRate); }

		void SetMaxUpdates(uint64_t maxUpdates);
		uint64_t GetMaxUpdates() const { return m_MaxUpdates; }

		Time GetElapsedTime() const { return m_Clock.GetElapsedTime(); }
		Time GetFrameTime() const { return m_FrameTime; }

		void LuaRegister(LuaRegister &luaObject);

		sol::state& GetLua() const { return *m_Lua; }
	private:
		void Initialize();
		void InitializeLua();
		
		bool OnWindowClose(WindowCloseEvent &event);
		bool OnWindowResize(WindowResizeEvent &event);
	};
}
