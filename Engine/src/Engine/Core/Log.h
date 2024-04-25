#pragma once
#include "Engine/Core/Base.h"

#include <spdlog/logger.h>

#define APPLICATION_LOGGER_NAME "Application"
#define ENGINE_LOGGER_NAME "Engine"
#define GL_LOGGER_NAME "OpenGL"
#define SCRIPT_LOGGER_NAME "Script"

#ifdef GAME_ENABLE_ASSERT
#define ASSERTION_LOGGER_NAME "Assertion"
#else
#define ASSERTION_LOGGER_NAME 
#endif

namespace Engine {
	class Log {
	public:
		static void Init(bool consoleOuput = true, bool FileOutput= true);

		static spdlog::logger& GetScriptLogger() { return *s_ScriptLogger; }
		static spdlog::logger& GetApplicationLogger() { return *s_ApplicationLogger; }
		static spdlog::logger& GetOpenGLLogger() { return *s_GLLogger; }
		static spdlog::logger& GetAssertionLogger() { return *s_AssertionLogger; }
		static spdlog::logger& GetEngineLogger() { return *s_EngineLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_AssertionLogger;
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ApplicationLogger;
		static std::shared_ptr<spdlog::logger> s_ScriptLogger;
		static std::shared_ptr<spdlog::logger> s_GLLogger;
	};
}

#define LOG_LOGGER_CALL(logger, level, ...) (logger).log(spdlog::source_loc{__FILE__, __LINE__, static_cast<const char*>(__FUNCTION__)}, level, __VA_ARGS__)

#define APPLICATION_LOGGER (::Engine::Log::GetApplicationLogger())
#define SCRIPT_LOGGER (::Engine::Log::GetApplicationLogger())
#define ASSERTION_LOGGER (::Engine::Log::GetApplicationLogger())
#define GL_LOGGER (::Engine::Log::GetApplicationLogger())
#define ENGINE_LOGGER (::Engine::Log::GetApplicationLogger())

#define LOG_TRACE(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define SCRIPT_LOG_TRACE(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define SCRIPT_LOG_DEBUG(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define SCRIPT_LOG_INFO(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::info, __VA_ARGS__)
#define SCRIPT_LOG_WARN(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define SCRIPT_LOG_ERROR(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::err, __VA_ARGS__)
#define SCRIPT_LOG_CRITICAL(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define GL_LOG_TRACE(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define GL_LOG_DEBUG(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define GL_LOG_INFO(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::info, __VA_ARGS__)
#define GL_LOG_WARN(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define GL_LOG_ERROR(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::err, __VA_ARGS__)
#define GL_LOG_CRITICAL(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define ENGINE_LOG_TRACE(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define ENGINE_LOG_DEBUG(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define ENGINE_LOG_INFO(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::info, __VA_ARGS__)
#define ENGINE_LOG_WARN(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define ENGINE_LOG_ERROR(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::err, __VA_ARGS__)
#define ENGINE_LOG_CRITICAL(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::critical, __VA_ARGS__)
