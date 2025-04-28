#include "pch.h"
#include "Engine/Core/Log.h"

#include "Engine/Core/CrashReporter.h"

#include <filesystem>
#include <ranges>

#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef GAME_DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace fs = std::filesystem;

namespace Engine {
	template <typename It>
	static std::shared_ptr<spdlog::logger> SetUpLogger(const std::string &name, const std::string& pattern, It begin, It end) {
		auto logger = MakeRef<spdlog::logger>(name, begin, end);

		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);
		logger->set_pattern(pattern);

		spdlog::register_logger(logger);

		return logger;
	}

	static std::shared_ptr<spdlog::logger> PrepareLogger(const std::string& name, const std::string& pattern, const std::string& fileName, bool consoleOutput, bool fileOutput) {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.reserve(3);

#ifdef GAME_DEBUG
		sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

		if (consoleOutput)
			sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

		if (fileOutput) {
			sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true));
		}

		return SetUpLogger(name, pattern, sinks.begin(), sinks.end());
	}

	static void CleanOldLogs(const std::string& folderPath, size_t maxFiles) {
		if (!fs::exists(folderPath) || fs::is_directory(folderPath))
			return;

		std::vector<fs::directory_entry> logFiles;

		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file())
				logFiles.push_back(entry);
		}

		std::ranges::sort(logFiles, [](const fs::directory_entry& a, const fs::directory_entry& b) {
			return fs::last_write_time(a) < fs::last_write_time(b);
			});

		while (logFiles.size() > maxFiles) {
			fs::remove(logFiles.front());
			logFiles.erase(logFiles.begin());
		}
	}

	Ref<spdlog::logger> Log::s_ScriptLogger = nullptr;
	Ref<spdlog::logger> Log::s_ApplicationLogger = nullptr;
	Ref<spdlog::logger> Log::s_GLLogger = nullptr;
	Ref<spdlog::logger> Log::s_EngineLogger = nullptr;

	void Log::Init(bool consoleOutput, bool fileOutput) {
		if (fileOutput && !fs::exists("Logs/Application")) fs::create_directories("Logs/Application");
		if (fileOutput && !fs::exists("Logs/Engine")) fs::create_directories("Logs/Engine");
		if (fileOutput && !fs::exists("Logs/GL")) fs::create_directories("Logs/GL");
		if (fileOutput && !fs::exists("Logs/Script")) fs::create_directories("Logs/Script");

		std::string timestamp = CrashReporter::GetCurrentTimestamp();
		std::ranges::replace(timestamp, ':', '-');

		const auto pattern = "%^- %D %T [%l] %n: %v%$";

		s_ApplicationLogger = PrepareLogger(APPLICATION_LOGGER_NAME, pattern, "Logs/Application/Application_" + timestamp + ".log", consoleOutput, fileOutput);
		s_EngineLogger = PrepareLogger(ENGINE_LOGGER_NAME, pattern, "Logs/Engine/Engine_" + timestamp + ".log", consoleOutput, fileOutput);
		s_GLLogger = PrepareLogger(GL_LOGGER_NAME, pattern, "Logs/GL/GL_" + timestamp + ".log", consoleOutput, fileOutput);
		s_ScriptLogger = PrepareLogger(SCRIPT_LOGGER_NAME, pattern, "Logs/Script/Script_" + timestamp + ".log", consoleOutput, fileOutput);
	}

	void Log::CleanAllLogDirectories(size_t maxFilesPerDirectory) {
		CleanOldLogs("Logs/Application", maxFilesPerDirectory);
		CleanOldLogs("Logs/Engine", maxFilesPerDirectory);
		CleanOldLogs("Logs/GL", maxFilesPerDirectory);
		CleanOldLogs("Logs/Script", maxFilesPerDirectory);
	}

	void Log::Shutdown() {
		spdlog::shutdown();
	}
}
