#pragma

#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"

extern Game::Application* Game::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Game::Log::Init();

	GAME_PROFILE_BEGIN_SESSION("startup", "EngineProfile-Startup.json");
	auto app = Game::CreateApplication({argc, argv});
	GAME_PROFILE_END_SESSION();

	int32_t exitCode = 0;

	GAME_PROFILE_BEGIN_SESSION("Initialize", "EngineProfile-Startup.json");
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
	GAME_PROFILE_END_SESSION();

	GAME_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Startup.json");
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
	GAME_PROFILE_END_SESSION();

	GAME_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Startup.json");
	delete app;
	GAME_PROFILE_END_SESSION();

	return exitCode;
}