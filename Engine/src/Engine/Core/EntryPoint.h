#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"

extern Engine::Application* Engine::CreateApplication(const ApplicationCommandLineArgs &args);

int main(int argc, char **argv) {
	Engine::Log::Init();

	auto app = Engine::CreateApplication({argc, argv});

	int32_t exitCode = 0;

	try {
		app->Initialize();
	}
	catch(std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception during initialization: {}", ex.what());
		app->Exit(-1);
	}
	catch(...) {
		LOG_ENGINE_CRITICAL("Unknown exception caught during initialization");
		app->Exit(-1);
	}

	try {
		exitCode = app->Run();
	}
	catch(std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception: {}", ex.what());
	}
	catch(...) {
		LOG_ENGINE_CRITICAL("Unknown exception caught");
	}

	try {
		delete app;
	}
	catch (std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception during freeing up memory: {}", ex.what());
	}
	catch (...) {
		LOG_ENGINE_CRITICAL("Unknown exception caught");
	}

	return exitCode;
}