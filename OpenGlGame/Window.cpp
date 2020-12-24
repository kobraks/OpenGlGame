#include "pch.h"
#include "Window.h"

#include "Assert.h"
#include "Log.h"

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "ApplicationEvent.h"

namespace Game
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char *description)
	{
		LOG_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const WindowProperties &properties)
	{
		Init(properties);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::SetVSync(bool enabled)
	{
		if(enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	OpenGlFunctions Window::GetFunctions() const
	{
		return m_Context->GetFunctions();
	}

	void Window::SetPosition(const uint32_t x, const uint32_t y)
	{
		glfwSetWindowPos(m_Window, x, y);
		m_Data.Position = Vector2u(x, y);
	}

	void Window::SetTitle(const std::string &title)
	{
		m_Data.Title = title;
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	void Window::Visible(const bool visible)
	{
		if(visible)
			glfwShowWindow(m_Window);
		else
			glfwHideWindow(m_Window);
	}

	bool Window::IsVisible() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_VISIBLE);
	}

	void Window::AttentionRequest()
	{
		glfwRequestWindowAttention(m_Window);
	}

	void Window::Init(const WindowProperties &props)
	{
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title  = props.Title;

		if(s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			ASSERT(success, "Unable to initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow(
		                            static_cast<int>(props.Width),
		                            static_cast<int>(props.Height),
		                            m_Data.Title.c_str(),
		                            nullptr,
		                            nullptr
		                           );
		++s_GLFWWindowCount;
		m_Context = Context::Create(*this);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowFocusCallback(
		                           m_Window,
		                           [](GLFWwindow *window, int focused)
		                           {
			                           WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                           if(focused)
			                           {
				                           WindowGainFocusEvent event;
				                           data.EventCallback(event);
			                           }
			                           else
			                           {
				                           WindowLostFocusEvent event;
				                           data.EventCallback(event);
			                           }
		                           }
		                          );

		glfwSetWindowPosCallback(
		                         m_Window,
		                         [](GLFWwindow *window, int x, int y)
		                         {
			                         WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                         data.Position    = Vector2u(x, y);

			                         WindowMovedEvent event(x, y);
			                         data.EventCallback(event);
		                         }
		                        );

		glfwSetWindowSizeCallback(
		                          m_Window,
		                          [](GLFWwindow *window, int width, int height)
		                          {
			                          WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                          data.Width       = width;
			                          data.Height      = height;

			                          WindowResizeEvent event(width, height);
			                          data.EventCallback(event);
		                          }
		                         );

		glfwSetWindowCloseCallback(
		                           m_Window,
		                           [](GLFWwindow *window)
		                           {
			                           WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                           WindowCloseEvent event;
			                           data.EventCallback(event);
		                           }
		                          );

		glfwSetKeyCallback(
		                   m_Window,
		                   [](GLFWwindow *window, int key, int scanCode, int action, int mods)
		                   {
			                   WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                   switch(action)
			                   {
				                   case GLFW_PRESS:
				                   {
					                   KeyPressedEvent event(key, 0);
					                   data.EventCallback(event);
					                   break;
				                   }

				                   case GLFW_RELEASE:
				                   {
					                   KeyReleasedEvent event(key);
					                   data.EventCallback(event);
					                   break;
				                   }

				                   case GLFW_REPEAT:
				                   {
					                   KeyPressedEvent event(key, 1);
					                   data.EventCallback(event);
					                   break;
				                   }
			                   }
		                   }
		                  );

		glfwSetCharCallback(
		                    m_Window,
		                    [](GLFWwindow *window, unsigned int keyCode)
		                    {
			                    WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			                    KeyTypedEvent event(keyCode);
			                    data.EventCallback(event);
		                    }
		                   );

		glfwSetMouseButtonCallback(
		                           m_Window,
		                           [](GLFWwindow *window, int button, int action, int mods)
		                           {
			                           WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			                           switch(action)
			                           {
				                           case GLFW_PRESS:
				                           {
					                           MouseButtonPressedEvent event(button);
					                           data.EventCallback(event);
					                           break;
				                           }
				                           case GLFW_RELEASE:
				                           {
					                           MouseButtonReleasedEvent event(button);
					                           data.EventCallback(event);
					                           break;
				                           }
			                           }
		                           }
		                          );

		glfwSetScrollCallback(
		                      m_Window,
		                      [](GLFWwindow *window, double xOffset, double yOffset)
		                      {
			                      WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			                      MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
			                      data.EventCallback(event);
		                      }
		                     );

		glfwSetCursorPosCallback(
		                         m_Window,
		                         [](GLFWwindow *window, double xPos, double yPos)
		                         {
			                         WindowData &data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			                         MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			                         data.EventCallback(event);
		                         }
		                        );
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if(s_GLFWWindowCount == 0)
			glfwTerminate();
	}
}
