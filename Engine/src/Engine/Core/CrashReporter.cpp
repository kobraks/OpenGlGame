#include "pch.h"
#include "CrashReporter.h"
#include "Engine/Core/Log.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <fmt/format.h>
#include <fmt/chrono.h>

namespace fs = std::filesystem;

namespace Engine::CrashReporter {
	static std::string s_ApplicationName = "Unknown";
	static std::string s_ApplicationVersion = "0.0.0";
	static std::string s_EngineVersion = "0.0.0";
	static std::string s_BuildType = "Unknown";

	static std::chrono::time_point<std::chrono::steady_clock> s_AppStartTime;

	std::string EscapeJsonString(const std::string& input) {
		std::ostringstream ss;
		for (const auto c : input) {
			switch (c) {
			case '"': ss << "\\\""; break;
			case '\\': ss << "\\\\"; break;
			case '\b': ss << "\\b"; break;
			case '\f': ss << "\\f"; break;
			case '\n': ss << "\\n"; break;
			case '\r': ss << "\\r"; break;
			case '\t': ss << "\\t"; break;
			default:
				if ('\x00' <= c && c <= '\x1f') {
					ss << "\\u"
						<< std::hex << std::setw(4) << std::setfill('0') << (int)c;
				}
				else {
					ss << c;
				}
			}
		}
		return ss.str();
	}
	void Initialize(const std::string& appName, const std::string& appVersion, const std::string& engineVersion, const std::string& buildType) {
		s_ApplicationName = appName;
		s_ApplicationVersion = appVersion;
		s_EngineVersion = engineVersion;
		s_BuildType = buildType;
		s_AppStartTime = std::chrono::steady_clock::now();
	}

	std::string GetCurrentTimestamp() {
		using namespace std::chrono;
		const auto now = system_clock::now();

		return fmt::format("{:%Y-%m-%d %H:%M:%S}", now);
	}

	std::string GetOSName() {
#if defined (_WIN32)
		return "Windows";
#elif defined (__linux__)
		return "Linux";
#elif defined (__APPLE__)
		return "MacOS";
#else
		return "Unknown OS";
#endif
	}

	std::string GetCPUArchitecture() {
#if defined(_M_X64) || defined(__x86_64__)
		return "x86_64";
#elif defined(_M_ARM64) || defined(__aarch64__)
		return "ARM64";
#else
		return "Unknown Architecture";
#endif
	}

	std::string GetCompiler() {
#if defined(__clang__)
		return "Clang";
#elif defined(__GNUC__)
		return "GCC";
#elif defined(_MSC_VER)
		return "MSVC";
#else
		return "Unknown Compiler";
#endif
	}

	unsigned int GetCPUCoreCount() {
		return std::thread::hardware_concurrency();
	}

	uint64_t GenerateCrashID() {
		using namespace std::chrono;
		return static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
	}

#ifdef _WIN32
	size_t GetTotalMemoryMB() {
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);

		return static_cast<size_t>(status.ullTotalPhys / (1024 * 1024));
	}
#endif

	void WriteCrashReport(const std::string& message) {
		const auto now = std::chrono::steady_clock::now();
		const auto uptimeSeconds = std::chrono::duration<double>(now - s_AppStartTime).count();

		const std::string timestamp = GetCurrentTimestamp();
		const auto crashID = GenerateCrashID();

		std::string safeTimestamp = timestamp;
		std::ranges::replace(safeTimestamp, ':', '-');

		const std::string crashReportFolder = "CrashReports";
		if (!fs::exists(crashReportFolder))
			fs::create_directory(crashReportFolder);

		const std::string fileName = crashReportFolder + "/CrashReport_" + safeTimestamp + "_" + std::to_string(crashID) + ".json";
		std::ofstream crashFile(fileName, std::ios::out | std::ios::trunc);
		if (!crashFile.is_open()) {
			LOG_ENGINE_CRITICAL("Failed to open crash report file!");
			return;
		}

		crashFile << "{\n";
		crashFile << "    \"CrashID\": " << crashID << ",\n";
		crashFile << "    \"Timestamp\": \"" << timestamp << "\",\n";
		crashFile << "    \"ApplicationName\": \"" << s_ApplicationName << "\",\n";
		crashFile << "    \"ApplicationVersion\": \"" << s_ApplicationVersion << "\",\n";
		crashFile << "    \"EngineVersion\": \"" << s_EngineVersion << "\",\n";
		crashFile << "    \"BuildType\": \"" << s_BuildType << "\",\n";
		crashFile << "    \"OS\": \"" << GetOSName() << "\",\n";
		crashFile << "    \"CPUArchitecture\": \"" << GetCPUArchitecture() << "\",\n";
		crashFile << "    \"Compiler\": \"" << GetCompiler() << "\",\n";
		crashFile << "    \"CPU_Cores\": " << GetCPUCoreCount() << ",\n";
#ifdef _WIN32
		crashFile << "    \"TotalMemoryMB\": " << GetTotalMemoryMB() << ",\n";
#endif
		crashFile << "    \"WorkingDirectory\": \"" << fs::current_path().string() << "\",\n";
		crashFile << "    \"UptimeSeconds\": " << uptimeSeconds << ",\n";
		crashFile << "    \"Message\": \"" << EscapeJsonString(message) << "\"\n";
		crashFile << "}\n";
	}

	void CleanOldCrashReports(size_t maxReports) {
		const std::string crashReportFolder = "CrashReports";

		if (!fs::exists(crashReportFolder))
			return;

		std::vector<fs::directory_entry> reports;

		for (const auto& entry : fs::directory_iterator(crashReportFolder)) {
			if (entry.is_regular_file()) {
				const auto& path = entry.path();
				if (path.filename().string().starts_with("CrashReport_") && path.extension() == ".txt") {
					reports.push_back(entry);
				}
			}
		}

		std::ranges::sort(reports, [](const fs::directory_entry& a, const fs::directory_entry& b) {
			return fs::last_write_time(a) < fs::last_write_time(b);
			});

		while (reports.size() > maxReports) {
			fs::remove(reports.front().path());
			reports.erase(reports.begin());
		}
	}

}