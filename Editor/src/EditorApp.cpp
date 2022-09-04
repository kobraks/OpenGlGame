#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Game
{
	class Editor: public Application
	{
	public:
		Editor(const ApplicationSpecification &spec) : Application(spec)
		{
		}

		void Initialize() override
		{
			Application::Initialize();

			PushLayer(MakePointer<EditorLayer>());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name            = "Editor";
		spec.CommandLineArgs = args;

		return new Editor(spec);
	}
}
