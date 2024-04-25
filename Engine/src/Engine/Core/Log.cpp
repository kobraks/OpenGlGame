#include "pch.h"
#include "Engine/Core/Log.h"

#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#ifdef GAME_DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

#include <filesystem>

namespace Engine {
	template <typename It>
	static std::shared_ptr<spdlog::logger> SetUpLogger(const std::string &name, It begin, It end) {
		auto logger = std::make_shared<spdlog::logger>(name, begin, end);

		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);

		spdlog::register_logger(logger);

		return logger;
	}

	std::shared_ptr<spdlog::logger> Log::s_AssertionLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_ScriptLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_ApplicationLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_GLLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_EngineLogger = nullptr;

	void Log::Init(bool consoleOut, bool fileOut) {
		const auto logPath = std::filesystem::current_path() / "Logs";
		if (!exists(logPath)) {
			std::filesystem::create_directories(logPath);
		}

		std::vector<spdlog::sink_ptr> sinks;
		sinks.reserve(3);

#ifdef GAME_DEBUG
		sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>())->set_pattern("- %D %T [%l] %n: %v");
#endif

		if (consoleOut) {
			sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_sink_mt>())->set_pattern("%^- %D %T [%l] %n: %v%$");
		}

		s_GLLogger = SetUpLogger(GL_LOGGER_NAME, std::begin(sinks), std::end(sinks));

		if (fileOut){
			sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Log.log", true))->set_pattern("- %D %T [%l] %n: %v");
		}

		s_ApplicationLogger = SetUpLogger(APPLICATION_LOGGER_NAME, std::begin(sinks), std::end(sinks));
		s_AssertionLogger = SetUpLogger(ASSERTION_LOGGER_NAME, std::begin(sinks), std::end(sinks));
		s_EngineLogger = SetUpLogger(ENGINE_LOGGER_NAME, std::begin(sinks), std::end(sinks));

		if (fileOut) {
			sinks[sinks.size() - 1] = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Script.log", true);
			sinks[sinks.size() - 1]->set_pattern("- %D %T [%l] %n: %v");
		}

		s_ScriptLogger = SetUpLogger(SCRIPT_LOGGER_NAME, std::begin(sinks), std::end(sinks));
	}

}