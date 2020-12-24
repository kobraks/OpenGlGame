#include "pch.h"
#include "Context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <exception>
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Window.h"

namespace Game
{
	Scope<Context> Context::Create(const Window& window)
	{
		return Scope<Context>(new Context(window.GetNativeWindow()));
	}

	bool Context::operator==(const Context &context) const
	{
		return m_ThreadId == context.m_ThreadId && m_WindowHandler == context.m_WindowHandler;
	}

	bool Context::operator!=(const Context &context) const
	{
		return m_ThreadId != context.m_ThreadId && m_WindowHandler == context.m_WindowHandler;
	}

	void Context::SwapBuffers()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(m_WindowHandler));
	}

	void Context::MakeCurrent()
	{
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandler));
	}

	Context::Context(void *windowHandler) : m_ThreadId(std::this_thread::get_id()), m_WindowHandler(windowHandler)
	{
		MakeCurrent();
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT(status, "Failed to initialize Glad!");

		LOG_INFO("OpenGL Info: ");
		LOG_INFO(" Vedor: {0}", glGetString(GL_VENDOR));
		LOG_INFO(" Renderer: {0}", glGetString(GL_RENDERER));
		LOG_INFO(" Version: {0}", glGetString(GL_VERSION));
	}
}
