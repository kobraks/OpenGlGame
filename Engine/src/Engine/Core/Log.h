#pragma once

#include "Engine/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define APPLICATION_LOGGER_NAME "Application"
#define OPENGL_LOGGER_NAME "OpenGl"
#define GL_LOGGER_NAME  OPENGL_LOGGER_NAME
#define SCRIPT_LOGGER_NAME "Script"
#define ASSERTION_LOGGER_NAME "Assertion"

namespace Game
{
	class Log
	{
		static Pointer<spdlog::logger> s_AssertionLogger;
		static Pointer<spdlog::logger> s_ScriptLogger;
		static Pointer<spdlog::logger> s_ApplicationLogger;
		static Pointer<spdlog::logger> s_OpenGlLogger;

	public:
		static void Init(bool consoleOutput = true, bool fileOutput = true);

		static Pointer<spdlog::logger>& GetScriptLogger() { return s_ScriptLogger; }
		static Pointer<spdlog::logger>& GetApplicationLogger() { return s_ApplicationLogger; }
		static Pointer<spdlog::logger>& GetOpenGLLogger() { return s_OpenGlLogger; }
		static Pointer<spdlog::logger>& GetAssertionLogger() { return s_AssertionLogger; }
	};
}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
OStream& operator<<(OStream &os, const glm::vec<L, T, Q> &vector)
{
	return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
OStream& operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix)
{
	return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
OStream& operator<<(OStream &os, glm::qua<T, Q> quaternio)
{
	return os << glm::to_string(quaternio);
}


#define LOG_LOGGER_CALL(logger, level, ...) (logger).log(spdlog::source_loc{__FILE__, __LINE__, static_cast<const char *>(__FUNCTION__)}, level, __VA_ARGS__)

#define APPLICATION_LOGGER (*::Game::Log::GetApplicationLogger())
#define SCRIPT_LOGGER (*::Game::Log::GetScriptLogger())
#define OPENGL_LOGGER (*::Game::Log::GetOpenGLLogger())
#define ASSERTION_LOGGER (*::Game::Log::GetAssertionLogger())

#define GL_LOGGER OPENGL_LOGGER

#define PRINT_TRACE(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define PRINT_DEBUG(...)LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define PRINT_INFO(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::info, __VA_ARGS__)
#define PRINT_WARN(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define PRINT_ERROR(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::err, __VA_ARGS__)
#define PRINT_CRITICAL(...) LOG_LOGGER_CALL(APPLICATION_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define LOG_TRACE(...) PRINT_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) PRINT_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) PRINT_INFO(__VA_ARGS__)
#define LOG_WARN(...) PRINT_WARN(__VA_ARGS__)
#define LOG_ERROR(...) PRINT_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) PRINT_CRITICAL(__VA_ARGS__)

#define PRINT_SCRIPT_TRACE(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define PRINT_SCRIPT_DEBUG(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define PRINT_SCRIPT_INFO(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::info, __VA_ARGS__)
#define PRINT_SCRIPT_WARN(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define PRINT_SCRIPT_ERROR(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::err, __VA_ARGS__)
#define PRINT_SCRIPT_CRITICAL(...) LOG_LOGGER_CALL(SCRIPT_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define SCRIPT_LOG_TRACE(...) PRINT_SCRIPT_TRACE(__VA_ARGS__)
#define SCRIPT_LOG_DEBUG(...) PRINT_SCRIPT_DEBUG(__VA_ARGS__)
#define SCRIPT_LOG_INFO(...) PRINT_SCRIPT_INFO(__VA_ARGS__)
#define SCRIPT_LOG_WARN(...) PRINT_SCRIPT_WARN(__VA_ARGS__)
#define SCRIPT_LOG_ERROR(...) PRINT_SCRIPT_ERROR(__VA_ARGS__)
#define SCRIPT_LOG_CRITICAL(...) PRINT_SCRIPT_CRITICAL(__VA_ARGS__)

#define PRINT_OPENGL_TRACE(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::trace, __VA_ARGS__)
#define PRINT_OPENGL_DEBUG(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::debug, __VA_ARGS__)
#define PRINT_OPENGL_INFO(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::info, __VA_ARGS__)
#define PRINT_OPENGL_WARN(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::warn, __VA_ARGS__)
#define PRINT_OPENGL_ERROR(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::err, __VA_ARGS__)
#define PRINT_OPENGL_CRITICAL(...) LOG_LOGGER_CALL(OPENGL_LOGGER, spdlog::level::critical, __VA_ARGS__)

#define OPENGL_LOG_TRACE(...) PRINT_OPENGL_TRACE(__VA_ARGS__)
#define OPENGL_LOG_DEBUG(...) PRINT_OPENGL_DEBUG(__VA_ARGS__)
#define OPENGL_LOG_INFO(...) PRINT_OPENGL_INFO(__VA_ARGS__)
#define OPENGL_LOG_WARN(...) PRINT_OPENGL_WARN(__VA_ARGS__)
#define OPENGL_LOG_ERROR(...) PRINT_OPENGL_ERROR(__VA_ARGS__)
#define OPENGL_LOG_CRITICAL(...) PRINT_OPENGL_CRITICAL(__VA_ARGS__)

#define GL_LOG_TRACE(...) PRINT_OPENGL_TRACE(__VA_ARGS__)
#define GL_LOG_DEBUG(...) PRINT_OPENGL_DEBUG(__VA_ARGS__)
#define GL_LOG_INFO(...) PRINT_OPENGL_INFO(__VA_ARGS__)
#define GL_LOG_WARN(...) PRINT_OPENGL_WARN(__VA_ARGS__)
#define GL_LOG_ERROR(...) PRINT_OPENGL_ERROR(__VA_ARGS__)
#define GL_LOG_CRITICAL(...) PRINT_OPENGL_CRITICAL(__VA_ARGS__)

#define PRINT_GL_TRACE(...) PRINT_OPENGL_TRACE(__VA_ARGS__)
#define PRINT_GL_DEBUG(...) PRINT_OPENGL_DEBUG(__VA_ARGS__)
#define PRINT_GL_INFO(...) PRINT_OPENGL_INFO(__VA_ARGS__)
#define PRINT_GL_WARN(...) PRINT_OPENGL_WARN(__VA_ARGS__)
#define PRINT_GL_ERROR(...) PRINT_OPENGL_ERROR(__VA_ARGS__)
#define PRINT_GL_CRITICAL(...) PRINT_OPENGL_CRITICAL(__VA_ARGS__)