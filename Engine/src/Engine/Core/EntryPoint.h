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

	GAME_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Startup.json");
	auto app = Game::CreateApplication({argc, argv});
	GAME_PROFILE_END_SESSION();

	GAME_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Startup.json");
	delete app;
	GAME_PROFILE_END_SESSION();
}