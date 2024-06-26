#include "pch.h"
#include "Engine/Renderer/Context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <exception>
#include <GLFW/glfw3.h>

#include "Assert.h"
#include "Engine/Core/Window.h"

namespace Game
{
	Context::~Context()
	{
		s_Context = nullptr;
		s_Contests[std::this_thread::get_id()] = nullptr;
	}

	Scope<Context> Context::Create(const Window& window)
	{
		return Scope<Context>(new Context(window.GetNativeWindow()));
	}

	OpenGLVersion Context::GetVersion() const
	{
		return m_Version;
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
		m_ThreadId = std::this_thread::get_id();
		m_Functions->MakeCurrent();
		s_Context = this;
		s_Contests[std::this_thread::get_id()] = this;
	}

	bool Context::IsCurrent() const
	{
		return std::this_thread::get_id() == m_ThreadId;
	}

	Context * Context::GetCurrentContext()
	{
		const auto context = s_Contests.find(std::this_thread::get_id());
		if (context != s_Contests.end())
			return context->second;
		
		return nullptr;
	}

	Context * Context::GetContext()
	{
		return s_Context;
	}

	Context::Context(void *windowHandler) : m_WindowHandler(windowHandler), m_ThreadId(std::this_thread::get_id())
	{
		MakeCurrent();
		const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		ASSERT(status, "Failed to initialize Glad!");
		if (!status)
			throw std::runtime_error("Failed to initialize Glad!");

		m_Functions = Scope<OpenGlFunctions>(new OpenGlFunctions(*this));

		LOG_INFO("OpenGL Info: ");
		LOG_INFO(" Vedor: {0}", (const char*)glGetString(GL_VENDOR));
		LOG_INFO(" Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LOG_INFO(" Version: {0}", (const char*)glGetString(GL_VERSION));

		m_Version = {m_Functions->GetInteger(GL_MAJOR_VERSION), m_Functions->GetInteger(GL_MINOR_VERSION)};
	}
}
