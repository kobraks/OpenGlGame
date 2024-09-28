#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Game {
	class Editor: public Engine::Application {
	public:
		Editor(const Engine::ApplicationSpecification &spec) : Application(spec) {}

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

		return new Game::Editor(spec);
	}
}
