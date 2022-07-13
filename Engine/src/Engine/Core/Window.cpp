#include "pch.h"
#include "Window.h"

#include "Assert.h"
#include "Log.h"

#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Devices/Cursor.h"
#include "Engine/Devices/Monitor.h"

namespace Game
{
	static uint8_t s_GLFWWindowCount = 0;
	static bool s_GLFWInitialized    = false;

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
		if (m_Cursor) 
			m_Cursor->Invalidate();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::SetInputMode(bool enabled, InputMode mode)
	{
		glfwSetInputMode(m_Window, static_cast<int>(mode), enabled ? GLFW_TRUE : GLFW_FALSE);
	}

	bool Window::GetInputMode(InputMode mode) const
	{
		return glfwGetInputMode(m_Window, static_cast<int>(mode)) == GLFW_TRUE ? true : false;
	}

	void Window::SetCursorMode(CursorMode mode)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<int>(mode));
	}
	
	bool Window::GetCursorMode(CursorMode mode)
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == static_cast<int>(mode);
	}
	
	CursorMode Window::GetCursorMode()
	{
		return static_cast<CursorMode>(glfwGetInputMode(m_Window, GLFW_CURSOR));
	}

	bool Window::IsRawMouseInputSupported()
	{
		InitializeGlfw();
		return glfwRawMouseMotionSupported() == GLFW_TRUE ? true : false;
	}

	void Window::SetSize(const Vector2u size)
	{
		SetSize(size.Width, size.Height);
	}

	void Window::SetSize(size_t width, size_t height)
	{
		glfwSetWindowSize(m_Window, static_cast<int>(width), static_cast<int>(height));
		m_Data.Width  = width;
		m_Data.Height = height;
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
		m_Data.X = x;
		m_Data.Y = y;
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

	Vector2u Window::GetRelativePosition(const Vector2u &position) const
	{
		return {m_Data.X + position.X, m_Data.Y + position.Y};
	}

	void Window::ToggleFullscreen()
	{
		const auto monitor = Monitor::GetPrimary();
		ToggleFullscreen(monitor, monitor.GetVideoMode());
	}

	void Window::ToggleFullscreen(const Monitor &monitor)
	{
		ToggleFullscreen(monitor, monitor.GetVideoMode());
	}

	void Window::ToggleFullscreen(const Monitor &monitor, const VideoMode &mode)
	{
		if(glfwGetWindowMonitor(m_Window) == nullptr)
		{
			m_Fullscreen     = true;
			m_BackupPosition = {m_Data.X, m_Data.Y};
			m_BackupSize     = {m_Data.Width, m_Data.Height};

			m_Monitor = MakeScope<Monitor>(monitor);
			
			glfwSetWindowMonitor(
			                     m_Window,
			                     static_cast<GLFWmonitor*>(monitor.GetNativePointer()),
			                     0,
			                     0,
			                     static_cast<int>(mode.Size.Width),
			                     static_cast<int>(mode.Size.Height),
			                     mode.RefreshRate
			                    );
		}
		else
		{
			m_Fullscreen = false;
			glfwSetWindowMonitor(m_Window, nullptr, m_BackupPosition.X, m_BackupPosition.Y, m_BackupSize.Width, m_BackupSize.Height, GLFW_DONT_CARE);
			SetPosition(m_BackupPosition);
			SetSize(m_BackupSize);

			m_Monitor = MakeScope<Monitor>();
		}
	}

	Cursor * Window::GetCursor() const
	{
		return &*m_Cursor;
	}

	void Window::SetCursor(std::unique_ptr<Cursor> &&cursor)
	{
		if (!cursor)
			return;
		
		if (m_Cursor)
			m_Cursor->Invalidate();
		
		m_Cursor = std::move(cursor);
		m_Cursor->Attach(this);
	}

	void Window::Invalidate()
	{
		glfwPostEmptyEvent();
	}

	bool Window::IsFullscreen() const
	{
		return m_Fullscreen;
	}

	void Window::Minimalize()
	{
		glfwIconifyWindow(m_Window);
	}
	void Window::Restore()
	{
		glfwRestoreWindow(m_Window);
	}
	void Window::Maximalize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void Window::InitializeGlfw()
	{
		if (s_GLFWInitialized)
			return;
		
		int success = glfwInit();
		ASSERT(success, "Unable to initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);

		if(success == GLFW_TRUE)
			s_GLFWInitialized = true;
	}

	void Window::Init(const WindowProperties &props)
	{
		// m_WindowContext();
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title  = props.Title;

		if(s_GLFWWindowCount == 0)
		{
			InitializeGlfw();
		}

		m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
		m_Context = Context::Create(*this);
		m_Monitor = MakeScope<Monitor>();
		
		Vector2i position;
		glfwGetWindowPos(m_Window, &position.X, &position.Y);
		m_Data.X = static_cast<uint32_t>(position.X);
		m_Data.Y = static_cast<uint32_t>(position.Y);

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
			                         data.X           = static_cast<uint32_t>(x);
			                         data.Y           = static_cast<uint32_t>(y);

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
		{
			glfwTerminate();
			s_GLFWInitialized = false;
		}
	}
}
