#pragma once
#include "Engine/Core/Base.h"
#include <string>}

namespace Engine::CrashReporter {
	struct CrashReporterConfig {
		std::string ApplicationName = "Unknown";
		std::string ApplicationVersion = "0.0.0";
		std::string EngineVersion = "0.0.0";
		std::string BuildType = "Unknown";
	};

	void Initialize(CrashReporterConfig config);
	void Initialize(const std::string& appName, const std::string& appVersion, const std::string& engineVersion, const std::string& buildType);
	void WriteCrashReport(const std::string& message);
	void CleanOldCrashReports(size_t maxReports = 10);

	std::string GetCurrentTimestamp();
	std::string GetOSName();
	std::string GetCPUArchitecture();
	std::string GetCompiler();
	unsigned int GetCPUCoreCount();
	uint64_t GenerateCrashID();

#ifdef _WIN32
	size_t GetTotalMemoryMB();
#endif
}

namespace Engine {
	CrashReporter::CrashReporterConfig InitializeCrashReporter();
}