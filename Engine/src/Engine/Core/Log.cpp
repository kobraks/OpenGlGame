#include "pch.h"
#include "Engine/Core/Log.h"

#include "Engine/Core/CrashReporter.h"

#include <filesystem>
#include <ranges>

#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef ENGINE_DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace fs = std::filesystem;

namespace Engine {
	template <typename It>
	static std::shared_ptr<spdlog::logger> SetUpLogger(const std::string& name, const std::string& pattern, It begin,
	                                                   It end,
	                                                   spdlog::level::level_enum loggingLevel = spdlog::level::trace,
	                                                   spdlog::level::level_enum flushLevel = spdlog::level::trace) {
		auto logger = MakeRef<spdlog::logger>(name, begin, end);

		logger->set_level(loggingLevel);
		logger->flush_on(flushLevel);
		logger->set_pattern(pattern);

		spdlog::register_logger(logger);

		return logger;
	}

	static std::shared_ptr<spdlog::logger> PrepareLogger(const std::string& name, const std::string& pattern,
	                                                     const std::string& fileName, bool consoleOutput,
	                                                     bool fileOutput,
	                                                     spdlog::level::level_enum loggingLevel = spdlog::level::trace,
	                                                     spdlog::level::level_enum flushLevel = spdlog::level::trace) {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.reserve(3);

#ifdef ENGINE_DEBUG
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
		if (!fs::exists(folderPath) || !fs::is_directory(folderPath))
			return;

		std::vector<fs::directory_entry> logFiles;

		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".log")
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


#ifdef ENGINE_DEBUG
		constexpr auto pattern = "%^[%D %T] [%l] [%n] [%s:%#] %v%$";
		constexpr auto logLevel = spdlog::level::trace;
#else
		constexpr auto pattern = "%^[%D %T] [%l] [%n] %v%$";
		constexpr auto logLevel = spdlog::level::info;
#endif
#ifdef ENGINE_ENABLE_OPENGL_DEBUG
		constexpr auto glLogLevel = spdlog::level::trace;
#else
		constexpr auto glLogLevel = spdlog::level::info;
#endif

		constexpr auto flushLevel = spdlog::level::err;

		s_ApplicationLogger = PrepareLogger(
			APPLICATION_LOGGER_NAME, pattern, "Logs/Application/Application_" + timestamp + ".log", consoleOutput,
			fileOutput, logLevel, flushLevel);
		s_EngineLogger = PrepareLogger(ENGINE_LOGGER_NAME, pattern, "Logs/Engine/Engine_" + timestamp + ".log",
		                               consoleOutput, fileOutput, logLevel, flushLevel);
		s_ScriptLogger = PrepareLogger(SCRIPT_LOGGER_NAME, pattern, "Logs/Script/Script_" + timestamp + ".log",
		                               consoleOutput, fileOutput, logLevel, flushLevel);
		s_GLLogger = PrepareLogger(GL_LOGGER_NAME, pattern, "Logs/GL/GL_" + timestamp + ".log", consoleOutput,
		                           fileOutput, glLogLevel, flushLevel);
	}

	void Log::CleanAllLogDirectories(size_t maxFilesPerDirectory) {
		CleanOldLogs("Logs/Application", maxFilesPerDirectory);
		CleanOldLogs("Logs/Engine", maxFilesPerDirectory);
		CleanOldLogs("Logs/GL", maxFilesPerDirectory);
		CleanOldLogs("Logs/Script", maxFilesPerDirectory);
	}

	void Log::Shutdown() {
		s_ScriptLogger.reset();
		s_GLLogger.reset();
		s_ApplicationLogger.reset();
		s_EngineLogger.reset();


		spdlog::shutdown();
	}
}
