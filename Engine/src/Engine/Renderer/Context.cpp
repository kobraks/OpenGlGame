#include "pch.h"
#include "Engine/Renderer/Context.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <fmt/std.h>


namespace Engine {
	Scope<Context> Context::Create(const Window *window) {
		ENGINE_ASSERT(window);

		return Scope<Context>(new Context(window->GetNativeWindow()));
	}

	void Context::SwapBuffers() {
		ENGINE_ASSERT(IsCurrent());
		glfwSwapBuffers(static_cast<GLFWwindow *>(m_Window));
	}

	bool Context::IsCurrent() const {
		return std::this_thread::get_id() == m_Thread;
	}


	void Context::MakeCurrent() {
		ENGINE_ASSERT(m_Thread == std::thread::id());

		if (m_Thread != std::thread::id()) {
			throw std::runtime_error(fmt::format("Context cannot be attached to current thread({}), context attached at: {}", std::this_thread::get_id(), m_Thread));
		}

		glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_Window));
		m_Thread = std::this_thread::get_id();
	}

	void Context::Detach() {
		if (IsCurrent()) {
			glfwMakeContextCurrent(nullptr);
			m_Thread = std::thread::id();
		}
	}

	Context::Context(void *windowHandler) {
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
