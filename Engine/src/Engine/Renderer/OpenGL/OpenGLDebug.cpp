#include "pch.h"
#include "Engine/Renderer/OpenGL/OpenGLDebug.h"

#include <glad/glad.h>

namespace Engine::OpenGLDebug {
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
			case GL_DEBUG_TYPE_PUSH_GROUP: return "PushGroup";
			case GL_DEBUG_TYPE_POP_GROUP: return "PopGroup";
			case GL_DEBUG_TYPE_OTHER: return "Other";
			default: return "Unknown";
			}
		}
	}

	void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length,
		const char* message, const void* userParam) {

		const auto sourceStr = Utils::GetStringForSource(source);
		const auto typeStr = Utils::GetStringForType(type);

		//const std::string formatted = fmt::format("[{}][{}][ID:{}]: {}", sourceStr, typeStr, id, message);
		spdlog::level::level_enum logLevel;

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			logLevel = spdlog::level::critical;
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			logLevel = spdlog::level::err;
			break;
		case GL_DEBUG_SEVERITY_LOW:
			logLevel = spdlog::level::warn;
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			logLevel = spdlog::level::trace;
			break;

		default:
			ENGINE_ASSERT(false, "Unknown OpenGL severity level!");
			return;
		}

		LOG_LOGGER_CALL(GL_LOGGER, logLevel, "[{}][{}][ID:{}]: {}", sourceStr, typeStr, id, message);
	}

	void Initialize() {
#ifdef ENGINE_ENABLE_OPENGL_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
	}
}