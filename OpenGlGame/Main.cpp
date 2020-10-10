#include "pch.h"
#include "Log.h"
#include "Uniform.h"


int main(int argc, char **argv)
{
	Game::Log::Init();

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
