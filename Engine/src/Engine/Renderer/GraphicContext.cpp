#include "pch.h"
#include "Engine/Renderer/GraphicContext.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <fmt/std.h>


namespace Engine {
	Scope<GraphicContext> GraphicContext::Create(const Window *window) {
		ENGINE_ASSERT(window);

		return Scope<GraphicContext>(new GraphicContext(window->GetNativeHandle()));
	}

	void GraphicContext::SwapBuffers() {
		ENGINE_ASSERT(IsCurrent());
		glfwSwapBuffers(static_cast<GLFWwindow *>(m_Window));
	}

	bool GraphicContext::IsCurrent() const {
		return std::this_thread::get_id() == m_Thread;
	}


	void GraphicContext::MakeCurrent() {
		ENGINE_ASSERT(m_Thread == std::thread::id());

		if (m_Thread != std::thread::id()) {
			throw std::runtime_error(fmt::format("GraphicContext cannot be attached to current thread({}), context attached at: {}", std::this_thread::get_id(), m_Thread));
		}

		glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_Window));
		m_Thread = std::this_thread::get_id();
	}

	void GraphicContext::Detach() {
		if (IsCurrent()) {
			glfwMakeContextCurrent(nullptr);
			m_Thread = std::thread::id();
		}
	}

	GraphicContext::GraphicContext(void *windowHandler) {
		m_Window = windowHandler;
		MakeCurrent();

		const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		ENGINE_ASSERT(status, "Failed to inintialize Glad!");


		if (!status)
			throw std::runtime_error("Failed to initialize Glad!");

		LOG_ENGINE_INFO("OpenGL Info: ");
		LOG_ENGINE_INFO(" Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		LOG_ENGINE_INFO(" Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LOG_ENGINE_INFO(" Version: {0}", (const char*)glGetString(GL_VERSION));

		glGetIntegerv(GL_MAJOR_VERSION, &m_Version.Major);
		glGetIntegerv(GL_MINOR_VERSION, &m_Version.Minor);
	}
}
