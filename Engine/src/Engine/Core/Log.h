#pragma once
#include "Engine/Core/Base.h"

#include <spdlog/logger.h>

#define APPLICATION_LOGGER_NAME "Application"
#define ENGINE_LOGGER_NAME "Engine"
#define GL_LOGGER_NAME "OpenGL"
#define SCRIPT_LOGGER_NAME "Script"

namespace Engine {
	class Log {
	public:
		static void Init(bool consoleOutput = true, bool fileOutput= true);
		static void CleanAllLogDirectories(size_t maxFilesPerDirectory = 10);
		static void Shutdown();

		static Ref<spdlog::logger> GetScriptLogger() { return s_ScriptLogger; }
		static Ref<spdlog::logger> GetApplicationLogger() { return s_ApplicationLogger; }
		static Ref<spdlog::logger> GetGLLogger() { return s_GLLogger; }
		static Ref<spdlog::logger> GetEngineLogger() { return s_EngineLogger; }

	private:
		static Ref<spdlog::logger> s_EngineLogger;
		static Ref<spdlog::logger> s_ApplicationLogger;
		static Ref<spdlog::logger> s_ScriptLogger;
		static Ref<spdlog::logger> s_GLLogger;
	};
}

#define LOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, static_cast<const char*>(__FUNCTION__)}, level, __VA_ARGS__)

#define APPLICATION_LOGGER (::Engine::Log::GetApplicationLogger())
#define SCRIPT_LOGGER (::Engine::Log::GetScriptLogger())
#define GL_LOGGER (::Engine::Log::GetGLLogger())
#define ENGINE_LOGGER (::Engine::Log::GetEngineLogger())

#define LOG_SEVERITY_TRACE spdlog::level::trace
#define LOG_SEVERITY_DEBUG spdlog::level::debug
#define LOG_SEVERITY_INFO spdlog::level::info
#define LOG_SEVERITY_WARN spdlog::level::warn
#define LOG_SEVERITY_ERROR spdlog::level::err
#define LOG_SEVERITY_CRITICAL spdlog::level::critical

#define LOG_TRACE(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_WARN(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_WARN, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, LOG_SEVERITY_CRITICAL, __VA_ARGS__)

#define LOG_SCRIPT_TRACE(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_TRACE, __VA_ARGS__)
#define LOG_SCRIPT_DEBUG(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_SCRIPT_INFO(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_SCRIPT_WARN(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_WARN, __VA_ARGS__)
#define LOG_SCRIPT_ERROR(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_SCRIPT_CRITICAL(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, LOG_SEVERITY_CRITICAL, __VA_ARGS__)

#define LOG_GL_TRACE(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_TRACE, __VA_ARGS__)
#define LOG_GL_DEBUG(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_GL_INFO(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_GL_WARN(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_WARN, __VA_ARGS__)
#define LOG_GL_ERROR(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_GL_CRITICAL(...) LOG_LOGGER_CALL(GL_LOGGER, LOG_SEVERITY_CRITICAL, __VA_ARGS__)

#define LOG_ENGINE_TRACE(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_TRACE, __VA_ARGS__)
#define LOG_ENGINE_DEBUG(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_ENGINE_INFO(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_INFO, __VA_ARGS__)
#define LOG_ENGINE_WARN(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_WARN, __VA_ARGS__)
#define LOG_ENGINE_ERROR(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_ENGINE_CRITICAL(...) LOG_LOGGER_CALL(ENGINE_LOGGER, LOG_SEVERITY_CRITICAL, __VA_ARGS__)
