#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

namespace Game
{
	class Editor: public Application
	{
	public:
		Editor(const ApplicationSpecification &spec) : Application(spec) { }
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name            = "Editor";
		spec.CommandLineArgs = args;

		return new Editor(spec);
	}
}
