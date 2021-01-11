#pragma once
#include <spdlog/spdlog.h>
#include "Types.h"

#define APPLICATION_LOGGER_NAME "Application"
#define OPENGL_LOGGER_NAME "OpenGl"
#define GL_LOGGER_NAME  OPENGL_LOGGER_NAME
#define SCRIPT_LOGGER_NAME "Script"
#define ASSERTION_LOGGER_NAME "Assertion"

namespace Game
{
	class Log
	{
	public:
		static void Init(bool consoleOutput = true, bool fileOutput = true);

		static Pointer<spdlog::logger>& GetScriptLogger() { return m_ScriptLogger; }
		static Pointer<spdlog::logger>& GetApplicationLogger() { return m_ApplicationLogger; }
		static Pointer<spdlog::logger>& GetOpenGLLogger() { return m_OpenGlLogger; }
		static Pointer<spdlog::logger>& GetAssertionLogger() { return m_AssertionLogger; }

	private:
		static Pointer<spdlog::logger> m_AssertionLogger;
		static Pointer<spdlog::logger> m_ScriptLogger;
		static Pointer<spdlog::logger> m_ApplicationLogger;
		static Pointer<spdlog::logger> m_OpenGlLogger;
	};
}


#define PRINT_TRACE(...) ::Game::Log::GetApplicationLogger()->trace(__VA_ARGS__)
#define PRINT_DEBUG(...)::Game::Log::GetApplicationLogger()->debug(__VA_ARGS__);
#define PRINT_INFO(...) ::Game::Log::GetApplicationLogger()->info(__VA_ARGS__)
#define PRINT_WARN(...) ::Game::Log::GetApplicationLogger()->warn(__VA_ARGS__)
#define PRINT_ERROR(...) ::Game::Log::GetApplicationLogger()->error(__VA_ARGS__)
#define PRINT_CRITICAL(...) ::Game::Log::GetApplicationLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...) PRINT_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) PRINT_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) PRINT_INFO(__VA_ARGS__)
#define LOG_WARN(...) PRINT_WARN(__VA_ARGS__)
#define LOG_ERROR(...) PRINT_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) PRINT_CRITICAL(__VA_ARGS__)

#define PRINT_SCRIPT_TRACE(...) ::Game::Log::GetScriptLogger()->trace(__VA_ARGS__)
#define PRINT_SCRIPT_DEBUG(...)::Game::Log::GetScriptLogger()->debug(__VA_ARGS__);
#define PRINT_SCRIPT_INFO(...) ::Game::Log::GetScriptLogger()->info(__VA_ARGS__)
#define PRINT_SCRIPT_WARN(...) ::Game::Log::GetScriptLogger()->warn(__VA_ARGS__)
#define PRINT_SCRIPT_ERROR(...) ::Game::Log::GetScriptLogger()->error(__VA_ARGS__)
#define PRINT_SCRIPT_CRITICAL(...) ::Game::Log::GetScriptLogger()->critical(__VA_ARGS__)

#define SCRIPT_LOG_TRACE(...) PRINT_SCRIPT_TRACE(__VA_ARGS__)
#define SCRIPT_LOG_DEBUG(...) PRINT_SCRIPT_DEBUG(__VA_ARGS__)
#define SCRIPT_LOG_INFO(...) PRINT_SCRIPT_INFO(__VA_ARGS__)
#define SCRIPT_LOG_WARN(...) PRINT_SCRIPT_WARN(__VA_ARGS__)
#define SCRIPT_LOG_ERROR(...) PRINT_SCRIPT_ERROR(__VA_ARGS__)
#define SCRIPT_LOG_CRITICAL(...) PRINT_SCRIPT_CRITICAL(__VA_ARGS__)

#define PRINT_OPENGL_TRACE(...) ::Game::Log::GetOpenGLLogger()->trace(__VA_ARGS__)
#define PRINT_OPENGL_DEBUG(...)::Game::Log::GetOpenGLLogger()->debug(__VA_ARGS__);
#define PRINT_OPENGL_INFO(...) ::Game::Log::GetOpenGLLogger()->info(__VA_ARGS__)
#define PRINT_OPENGL_WARN(...) ::Game::Log::GetOpenGLLogger()->warn(__VA_ARGS__)
#define PRINT_OPENGL_ERROR(...) ::Game::Log::GetOpenGLLogger()->error(__VA_ARGS__)
#define PRINT_OPENGL_CRITICAL(...) ::Game::Log::GetOpenGLLogger()->critical(__VA_ARGS__)

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
