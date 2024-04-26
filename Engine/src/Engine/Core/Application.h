#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Window.h"

int main(int arc, char **argv);

namespace Engine {
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

		void OnEvent(Event &event);
		void PushLayer(Pointer<Layer> layer);
		void PushOverlay(Pointer<Layer> layer);

		void Close() { Exit(0); }
		void Exit(int exitCode);

		Window& GetWindow() const { return *m_Window; }

		int Run();

		virtual void ProcessArgs(const ApplicationCommandLineArgs &args);
		virtual void Initialize();

		static Application& Get() { return *s_Instance; }
	private:

	private:
		Scope<Window> m_Window;

		int m_ExitCode = 0;

		static Application *s_Instance;
	};

	Application* CreateApplication(const ApplicationCommandLineArgs &args);
}