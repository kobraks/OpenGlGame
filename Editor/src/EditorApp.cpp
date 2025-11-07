#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Editor {
	class EditorApp: public Engine::Application {
	public:
		EditorApp(const Engine::ApplicationSpecification &spec) : Application(spec) {}

		void Initialize() override {
			Application::Initialize();

			PushLayer(Engine::MakeRef<EditorLayer>());
		}
	};
}

namespace Engine {
	Application* Engine::CreateApplication(const ApplicationCommandLineArgs &args) {
		ApplicationSpecification spec;
		spec.Name            = "Editor";
		spec.CommandLineArgs = args;
		spec.Maximized = true;
		

		return new Editor::EditorApp(spec);
	}

	Engine::CrashReporter::CrashReporterConfig Engine::InitializeCrashReporter() {
		CrashReporter::CrashReporterConfig config;

		config.ApplicationName = "Editor";
		config.ApplicationVersion = "0.0.1";
		config.BuildType = "Dev";
		config.EngineVersion = "0.0.1";

		return config;
	}
}
