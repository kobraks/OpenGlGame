#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/TimeStepController.h"

#include "Engine/Layers/LayerStack.h"
#include "Engine/Layers/ImGuiLayer.h"

int main(int arc, char **argv);

namespace Engine {
	class WindowResizeEvent;
	class WindowCloseEvent;

	struct ApplicationCommandLineArgs {
		size_t Count = 0;
		char **Args = nullptr;

		std::string_view operator[](size_t index) const {
			ENGINE_ASSERT(index < Count);

			if (index >= Count) {
				throw std::out_of_range("Out of range");
			}

			return Args[index];
		}

		std::string_view At(size_t index) const {
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
		bool FullWindow = false;

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

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		TimeStepController& GetTimeStepController() { return m_TimeStepController; }
		const TimeStepController& GetTimeStepController() const { return m_TimeStepController; }

		std::string_view GetCommandLineArg(size_t i) const { return m_Arguments[i]; }
		size_t GetCommandLineArgCount() const { return m_Arguments.size(); }
		const std::vector<std::string> GetCommandLineArgs() const { return m_Arguments; }

		Time GetFrameTime() const { return m_TimeStepController.GetFrameDelta(); }
		Time GetElapsedTime() const { return m_AppClock.GetElapsedTime(); }

		static Application& Get() { return *s_Instance; }

		Ref<ImGuiLayer> GetImGuiLayer() const { return m_ImGuiLayer;  };

	protected:
		virtual void InitializeSettings();
		virtual void InitializeLua();

	private:
		bool OnWindowClose(const WindowCloseEvent &event);
		bool OnWindowResize(const WindowResizeEvent &event);
	private:
		Scope<Window> m_Window;

		ApplicationSpecification m_Specification;
		TimeStepController m_TimeStepController;

		LayerStack m_LayerStack;

		int m_ExitCode = 0;

		bool m_Run = true;
		bool m_Minimized = false;

		Ref<ImGuiLayer> m_ImGuiLayer = nullptr;

		Clock m_AppClock;

		std::vector<std::string> m_Arguments;

		static Application *s_Instance;

		friend int ::main(int argc, char **argv);
	};

	Application* CreateApplication(const ApplicationCommandLineArgs &args);
}