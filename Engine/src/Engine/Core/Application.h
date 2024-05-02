#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Clock.h"

#include "Engine/Layers/LayerStack.h"

int main(int arc, char **argv);

namespace Engine {
	class WindowResizeEvent;
	class WindowCloseEvent;
	class ImGuiLayer;

	struct ApplicationCommandLineArgs {
		int Count = 0;
		char **Args = nullptr;

		std::string_view operator[](size_t index) const {
			ENGINE_ASSERT(index < Count);

			if (index >= Count) {
				throw std::out_of_range("Out of range");
			}

			return Args[index];
		}

		char ** begin() { return Args; }
		char ** end() { return Args + Count; }

		char ** begin() const { return Args; }
		char ** end() const { return Args + Count; }
	};

	struct ApplicationSpecification {
		std::string Name = "Application";
		std::filesystem::path WorkingDirectory;

		bool FullScreen = false;
		Vector2u WindowSize{1027, 768};

		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application {
	public:
		explicit Application(const ApplicationSpecification &specs);
		virtual ~Application();

		virtual void OnEvent(Event &event);

		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);

		void Close() { Exit(0); }
		void Exit(int exitCode);

		Window& GetWindow() const { return *m_Window; }

		int Run();

		virtual void ProcessArgs(const ApplicationCommandLineArgs &args);
		virtual void Initialize();

		static Application& Get() { return *s_Instance; }

	protected:
		virtual void InitializeSettings();
		virtual void InitializeLua();

	private:
		bool OnWindowClose(const WindowCloseEvent &event);
		bool OnWindowResize(const WindowResizeEvent &event);
	private:
		Scope<Window> m_Window;

		ApplicationSpecification m_Specification;

		LayerStack m_LayerStack;

		int m_ExitCode = 0;

		bool m_Run = true;
		bool m_Minimalized = false;

		Ref<ImGuiLayer> m_ImGuiLayer = nullptr;

		uint64_t m_MaxUpdates = 60;
		uint32_t m_UpdateRate = static_cast<uint32_t>(1000.f / 60.f);

		Time m_FrameTime = Time::Zero;

		Clock m_Clock;

		std::vector<std::string> m_Arguments;

		static Application *s_Instance;

		friend int ::main(int argc, char **argv);
	};

	Application* CreateApplication(const ApplicationCommandLineArgs &args);
}