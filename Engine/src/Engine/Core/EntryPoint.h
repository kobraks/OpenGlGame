#pragma

#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"

extern Game::Application* Game::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Game::Log::Init();

	auto app = Game::CreateApplication({argc, argv});

	int32_t exitCode = 0;

	try
	{
		app->Initialize();
	}
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
		app->Exit(-1);
	}
	catch(...)
	{
		LOG_CRITICAL("Unknown exception catched");
		system("pause");
		app->Exit(-1);
	}

	try
	{
		exitCode = app->Run();
	}
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
	}
	catch(...)
	{
		LOG_CRITICAL("Unknown exception catched");
		system("pause");
	}

	delete app;

	return exitCode;
}