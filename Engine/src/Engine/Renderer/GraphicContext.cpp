#include "pch.h"
#include "GraphicContext.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <fmt/std.h>

namespace Engine {
	GraphicContext::~GraphicContext() {
		Detach();

		if (IsShared())
			glfwDestroyWindow(static_cast<GLFWwindow*>(m_WindowHandle));
	}

	Scope<GraphicContext> GraphicContext::Create(const Window *window) {
		ENGINE_ASSERT(window);

		return Scope<GraphicContext>(new GraphicContext(window->GetNativeHandle()));
	}

	Scope<GraphicContext> GraphicContext::CreateShared(const GraphicContext* sharedWith) {
		ENGINE_ASSERT(sharedWith);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		auto hiddenWindow = Window::Create(1, 1, "", nullptr, sharedWith->m_WindowHandle);

		auto context = Scope<GraphicContext>(new GraphicContext(hiddenWindow));
		context->m_MainWindowHandle = sharedWith->m_WindowHandle;

		return context;
	}

	void GraphicContext::SwapBuffers() {
		ENGINE_ASSERT(IsCurrent());
		glfwSwapBuffers(static_cast<GLFWwindow *>(m_WindowHandle));
	}

	bool GraphicContext::IsCurrent() const {
		return std::this_thread::get_id() == m_Thread;
	}

	bool GraphicContext::IsValid() const {
		return m_WindowHandle != nullptr;
	}


	void GraphicContext::MakeCurrent() {
		while (m_ContextBusy.test_and_set(std::memory_order_acquire))
			m_ContextBusy.wait(true, std::memory_order_relaxed);

		if (m_Thread != std::thread::id()) {
			ENGINE_ASSERT(m_Thread == std::this_thread::get_id(), "Context is already current on another thread!");
		}

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandle));
		m_Thread = std::this_thread::get_id();

		m_ContextBusy.clear(std::memory_order_release);
	}

	void GraphicContext::Detach() {
		while (m_ContextBusy.test_and_set(std::memory_order_acquire))
			m_ContextBusy.wait(true, std::memory_order_relaxed);

		if (m_Thread == std::this_thread::get_id()) {
			glfwMakeContextCurrent(nullptr);
			m_Thread = std::thread::id();
		}

		m_ContextBusy.clear(std::memory_order_release);
	}

	void GraphicContext::Init(void* windowHandle) {
		m_WindowHandle = windowHandle;
		MakeCurrent();

		const int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		ENGINE_ASSERT(status, "Failed to initialize Glad!");

		if (!status)
			throw std::runtime_error("Failed to initialize GLAD!");

		LOG_ENGINE_INFO("OpenGL Info:");
		LOG_ENGINE_INFO(" Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		LOG_ENGINE_INFO(" Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		LOG_ENGINE_INFO(" Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		glGetIntegerv(GL_MAJOR_VERSION, &m_Version.Major);
		glGetIntegerv(GL_MINOR_VERSION, &m_Version.Minor);
	}

	bool GraphicContext::IsThreadBound() const {
		return m_Thread != std::thread::id{};
	}

	GraphicContext::GraphicContext(void *windowHandle) {
		Init(windowHandle);
	}
}
