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
		static void Init(bool consoleOuput = true, bool FileOutput= true);

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

#define LOG_TRACE(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define LOG_SCRIPT_TRACE(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define LOG_SCRIPT_DEBUG(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define LOG_SCRIPT_INFO(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::info, __VA_ARGS__)
#define LOG_SCRIPT_WARN(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define LOG_SCRIPT_ERROR(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::err, __VA_ARGS__)
#define LOG_SCRIPT_CRITICAL(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define LOG_GL_TRACE(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define LOG_GL_DEBUG(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define LOG_GL_INFO(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::info, __VA_ARGS__)
#define LOG_GL_WARN(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define LOG_GL_ERROR(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::err, __VA_ARGS__)
#define LOG_GL_CRITICAL(...) LOG_LOGGER_CALL(GL_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define LOG_ENGINE_TRACE(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define LOG_ENGINE_DEBUG(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define LOG_ENGINE_INFO(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::info, __VA_ARGS__)
#define LOG_ENGINE_WARN(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define LOG_ENGINE_ERROR(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::err, __VA_ARGS__)
#define LOG_ENGINE_CRITICAL(...) LOG_LOGGER_CALL(ENGINE_LOGGER, spdlog::level::critical, __VA_ARGS__)
