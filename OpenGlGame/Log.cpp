#include "pch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _DEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

#include <filesystem>

namespace Game
{
	template <typename Iterator>
	static Pointer<spdlog::logger> SetUpLogger(const std::string &name, Iterator begin, Iterator end)
	{
		auto logger = std::make_shared<spdlog::logger>(name, begin, end);
		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);

		spdlog::register_logger(logger);

		return logger;
	}

	Pointer<spdlog::logger> Log::m_ApplicationLogger = nullptr;
	Pointer<spdlog::logger> Log::m_ScriptLogger      = nullptr;
	Pointer<spdlog::logger> Log::m_OpenGlLogger      = nullptr;
	Pointer<spdlog::logger> Log::m_AssertionLogger   = nullptr;

	void Log::Init(bool consoleOutput, bool fileOutput)
	{
		const auto logPath = std::filesystem::current_path() / "Logs";
		if(!exists(logPath))
		{
			create_directories(logPath);
		}

		std::vector<spdlog::sink_ptr> sinks;
		sinks.reserve(3);

#ifdef _DEBUG
		sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>())->set_pattern("- %D %T [%l] %n: %v");
#endif

		if(consoleOutput)
			sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())->set_pattern("%^- %D %T [%l] %n: %v%$");

		if(fileOutput)
			sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Log.log", true))->set_pattern("- %D %T [%l] %n: %v");

		m_ApplicationLogger = SetUpLogger(APPLICATION_LOGGER_NAME, std::begin(sinks), std::end(sinks));
		m_OpenGlLogger = SetUpLogger(GL_LOGGER_NAME, std::begin(sinks), std::end(sinks));
		m_AssertionLogger = SetUpLogger(ASSERTION_LOGGER_NAME, std::begin(sinks), std::end(sinks));

		if (fileOutput)
		{
			sinks[sinks.size() - 1] = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Script.log", true);
			sinks[sinks.size() - 1]->set_pattern("- %D %T [%l] %n: %v");
		}

		m_ScriptLogger = SetUpLogger(SCRIPT_LOGGER_NAME, std::begin(sinks), std::end(sinks));
	}
}
