#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"

#include "Engine/Core/CrashReporter.h"

extern Engine::Application* Engine::CreateApplication(const ApplicationCommandLineArgs &args);

int main(int argc, char **argv) {
	Engine::CrashReporter::Initialize("OpenGLGame", "0.1.0-alpha", "Engine 0.3.2-dev", "Debug"); //TODO TEMPLORARY
	Engine::CrashReporter::CleanOldCrashReports();
	Engine::Log::Init();

	auto app = Engine::CreateApplication({static_cast<size_t>(argc), argv});

	int32_t exitCode = 0;

	try {
		app->ProcessArgs({ static_cast<size_t>(argc), argv });
		app->Initialize();
	}
	catch(std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception during initialization: {}", ex.what());
		Engine::CrashReporter::WriteCrashReport(fmt::format("Uncaught exception during initialization: {}", ex.what()));
		app->Exit(-1);
	}
	catch(...) {
		LOG_ENGINE_CRITICAL("Unknown exception caught during initialization");
		Engine::CrashReporter::WriteCrashReport("Uncaught exception during initialization!");
		app->Exit(-1);
	}

	try {
		exitCode = app->Run();
	}
	catch(std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception: {}", ex.what());
		Engine::CrashReporter::WriteCrashReport(fmt::format("Uncaught exception: {}", ex.what()));
		exitCode = -1;
	}
	catch(...) {
		LOG_ENGINE_CRITICAL("Unknown exception caught!");
		Engine::CrashReporter::WriteCrashReport("Uncaught exception caught!");
		exitCode = -1;
	}

	try {
		if (app)
			delete app;
	}
	catch (std::exception &ex) {
		LOG_ENGINE_CRITICAL("Uncaught exception during freeing up memory: {}", ex.what());
		Engine::CrashReporter::WriteCrashReport(fmt::format("Uncaught exception during freeing up memory: {}", ex.what()));
		exitCode = -1;
	}
	catch (...) {
		LOG_ENGINE_CRITICAL("Unknown exception during freeing up memory!");
		Engine::CrashReporter::WriteCrashReport("Unknown exception during freeing up memory!");
		exitCode = -1;
	}

	return exitCode;
}