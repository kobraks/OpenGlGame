#include "pch.h"

#include "Application.h"

int main(int argc, char **argv)
{
	Game::Application game;
	game.ProcessArgs(argc, argv);
	
	int32_t exitCode = 0;

	try
	{
		exitCode = game.Run();
	}
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
		system("pause");
		exitCode = EXIT_FAILURE;
	}

	LOG_INFO("Exited with code: {}", exitCode);

	return exitCode;
}
