#include "pch.h"
#include "Engine/Renderer/RendererCommand.h"

#include "glad/glad.h"

namespace Engine {
	void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length,
	                           const char* message, const void* userParam) {
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: LOG_GL_CRITICAL(message);
			return;
		case GL_DEBUG_SEVERITY_MEDIUM: LOG_GL_ERROR(message);
			return;
		case GL_DEBUG_SEVERITY_LOW: LOG_GL_WARN(message);
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_GL_TRACE(message);
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
