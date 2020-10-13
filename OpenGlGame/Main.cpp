#include "pch.h"
#include "Log.h"
#include "Uniform.h"

#include "Color.h"

int main(int argc, char **argv)
{
	Game::Log::Init();


	LOG_INFO("Color: {}", sizeof(Game::Color));
	
	int32_t exitCode = EXIT_SUCCESS;

	try { }
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
		system("pause");
		exitCode = EXIT_FAILURE;
	}

	LOG_INFO("Exited with code: {}", exitCode);

	return 0;
}
