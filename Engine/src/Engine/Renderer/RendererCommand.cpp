#include "pch.h"
#include "Engine/Renderer/RendererCommand.h"

#include "Engine/Renderer/OpenGL/OpenGLDebug.h"

#include <glad/glad.h>

#include <string_view>

namespace Engine {

	void RendererCommand::Init() {
		OpenGLDebug::Initialize();

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
