#pragma once

#include "Engine/Core/Base.h"

#include "Engine/Core/Clock.h"
#include "Engine/Core/Window.h"

#include "Engine/Layers/LayerStack.h"

#include "Engine/Property/PropertyManager.h"

#include "Engine/Utils/Shortcut.h"

#include <vector>
#include <stdexcept>

// #include "ThreadPool.h"

int main(int argc, char** argv);

namespace Game
{
	class WindowResizeEvent;
	class WindowCloseEvent;
	class LuaRegister;

	class ImGuiLayer;

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char **Args = nullptr;

		const char* operator[](int index) const
		{
			ASSERT(index < Count);
			if (index < 0 || index >= Count)
			{
				throw std::out_of_range("OutOfRange");
			}

			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Application";
		std::string WorkingDirectory;

		bool Fullscreen = false;
		Vector2u WindowSize { 800, 600 };

		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
		ApplicationSpecification m_Specification;

		Scope<Window> m_Window;
		Scope<sol::state> m_Lua;
		Scope<PropertyManager> m_Properties;
		// Scope<ThreadPool> m_ThreadPool;

		bool m_Running     = true;
		bool m_Minimalized = false;

		LayerStack m_LayerStack;

		Pointer<ImGuiLayer> m_ImGuiLayer = nullptr;

		int m_ExitCode = 0;

		uint64_t m_MaxUpdates = 60;
		uint32_t m_UpdateRate = static_cast<uint32_t>(1000.f / 60.f);

		Time m_FrameTime  = Time::Zero;

		Clock m_Clock;

		std::vector<std::string> m_Arguments;
		std::vector<Shortcut> m_Shortcuts;

		static Application *s_Instance;

	public:
		explicit Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void OnEvent(Event &event);
		void PushLayer(Pointer<Layer> layer);
		void PushOverlay(Pointer<Layer> overlay);

		void RegisterShortcut(const Shortcut& shortcut);

		// ThreadPool& GetThreadPool() const { return *m_ThreadPool; }
		Window& GetWindow() const { return *m_Window; }

		void Close() { Exit(0); }
		void Exit(int exitCode);

		static Application& Get() { return *s_Instance; }
		int Run();

		virtual void ProcessArgs(const ApplicationCommandLineArgs &args);

		void SetUpdateRate(float rate);
		float GetUpdateRate() const { return 1000.f / static_cast<float>(m_UpdateRate); }

		void SetMaxUpdates(uint64_t maxUpdates);
		uint64_t GetMaxUpdates() const { return m_MaxUpdates; }

		Time GetElapsedTime() const { return m_Clock.GetElapsedTime(); }
		Time GetFrameTime() const { return m_FrameTime; }

		PropertyManager& GetProperties() const { return *m_Properties; }

		void LuaRegister(LuaRegister &luaObject);

		sol::state& GetLua() const { return *m_Lua; }
		virtual void Initialize();
	private:
		void InitializeLua();

		void InitializeSettings();
		
		bool OnWindowClose(WindowCloseEvent &event);
		bool OnWindowResize(WindowResizeEvent &event);

		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
