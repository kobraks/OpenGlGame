#include "pch.h"
#include "Engine/Renderer/RendererCommand.h"

#include "glad/glad.h"

#include <string_view>

namespace Engine {
	namespace Utils {
		constexpr std::string_view GetStringForSource(GLenum source) {
			switch (source) {
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WindowSystem";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "ShaderCompiler";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "ThirdParty";
			case GL_DEBUG_SOURCE_APPLICATION: return "Application";
			case GL_DEBUG_SOURCE_OTHER: return "Other";
			default: return "Unknown";
			}
		}
		
		constexpr std::string_view GetStringForType(GLenum type) {
			switch (type) {
			case GL_DEBUG_TYPE_ERROR: return "Error";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UndefinedBehavior";
			case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
			case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
			case GL_DEBUG_TYPE_MARKER: return "Marker";
			case GL_DEBUG_TYPE_OTHER: return "Other";
			default: return "Unknown";
			}
		}
	}

	void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length,
	                           const char* message, const void* userParam) {

		const auto sourceStr = Utils::GetStringForSource(source);
		const auto typeStr = Utils::GetStringForType(type);

		const std::string formatted = fmt::format("[{}][{}][ID:{}]: {}", sourceStr, typeStr, id, message);

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: LOG_GL_CRITICAL(formatted);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM: LOG_GL_ERROR(formatted);
			return;
		case GL_DEBUG_SEVERITY_LOW: LOG_GL_WARN(formatted);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_GL_TRACE(formatted);
			return;
		}

		ENGINE_ASSERT(false, "Unknown severity level!");
	}

	void RendererCommand::Init() {
#ifdef GAME_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void RendererCommand::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void RendererCommand::SetClearColor(const Color& color) {
		const auto fColor = color.ToFloat();

		glClearColor(fColor.r, fColor.g, fColor.b, fColor.a);
	}

	void RendererCommand::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
