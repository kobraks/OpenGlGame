#pragma once

#include <vector>

#include "Types.h"

#include "Window.h"
#include "LayerStack.h"
#include "Event.h"
#include "ApplicationEvent.h"

#include "Time.h"
#include "Clock.h"

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

		float m_Frequency = 60.f;

		int m_ExitCode = 0;

		uint64_t m_MaxUpdates = 60;

		Time m_UpdateRate = Seconds(1.f / 60.f);
		Time m_FrameTime  = Time::Zero;

		Clock m_Clock;

		std::vector<std::string> m_Arguments;

		static Application *s_Instance;

	public:
		Application();
		~Application();

		void OnEvent(Event &event);
		void PushLayer(Pointer<Layer> layer);
		void PushOverlay(Pointer<Layer> overlay);

		Window& GetWindow() const { return *m_Window; }

		void Close() { Exit(0); }
		void Exit(int exitCode);

		static Application& Get() { return *s_Instance; }
		int Run();

		void ProcessArgs(int argc, char **argv);

		void SetUpdateRate(float frequency);
		float GetUpdateRate() const { return m_Frequency; }

		void SetMaxUpdates(uint64_t maxUpdates);
		uint64_t GetMaxUpdates() const { return m_MaxUpdates; }

		Time GetElapsedTime() const { return m_Clock.GetElapsedTime(); }
		Time GetFrameTime() const { return m_FrameTime; }

		void LuaRegister(LuaRegister &luaObject);
	private:
		void Initialize();

		bool OnWindowClose(WindowCloseEvent &event);
		bool OnWindowResize(WindowResizeEvent &event);
	};
}
