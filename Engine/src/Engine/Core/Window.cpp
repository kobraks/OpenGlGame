#include "pch.h"
#include "Engine/Core/Window.h"

#include "Engine/Renderer/Context.h"

#include "Engine/Devices/Cursor.h"
#include "Engine/Devices/Monitor.h"

#include <GLFW/glfw3.h>

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine {
	static uint8_t s_GLFWWindowCount = 0;
	static bool s_GLFWInitialized    = false;

	static void GLFWErrorCallback(int error, const char *desc) {
		LOG_ENGINE_ERROR("GLFW Error ({0}): {1}", error, desc);
	}

	Scope<Window> Window::Create(const WindowProperties &props) {
		return Scope<Window>(new Window(props));
	}

	Window::~Window() {
		Shutdown();
		if(m_Cursor)
			m_Cursor->Invalidate();
	}

	void Window::OnUpdate() {
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::SetInputMode(InputMode mode, bool enabled) {
		glfwSetInputMode(static_cast<GLFWwindow*>(m_Window), static_cast<int>(mode), enabled ? GLFW_TRUE : GLFW_FALSE);
	}

	void Window::SetCursorMode(CursorMode mode) {
		glfwSetInputMode(static_cast<GLFWwindow*>(m_Window), GLFW_CURSOR, static_cast<int>(mode));
	}

	void Window::SetSize(const Vector2u &size) {
		SetSize(size.Width, size.Height);
	}

	void Window::SetSize(uint32_t width, uint32_t height) {
		glfwSetWindowSize(static_cast<GLFWwindow*>(m_Window), static_cast<int>(width), static_cast<int>(height));

		m_Data.Width  = width;
		m_Data.Height = height;
	}

	void Window::SetPos(int32_t x, int32_t y) {
		glfwSetWindowPos(static_cast<GLFWwindow*>(m_Window), x, y);

		m_Data.X = x;
		m_Data.Y = y;
	}

	void Window::SetPos(const Vector2i &pos) {
		SetPos(pos.X, pos.Y);
	}

	void Window::SetVSync(bool enabled) {
		if(enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	void Window::SetTitle(std::string title) {
		glfwSetWindowTitle(static_cast<GLFWwindow*>(m_Window), title.c_str());
		m_Data.Title = std::move(title);
	}

	void Window::Visible(bool visible) {
		if(visible)
			glfwShowWindow(static_cast<GLFWwindow*>(m_Window));
		else
			glfwHideWindow(static_cast<GLFWwindow*>(m_Window));
	}

	CursorMode Window::GetCursorMode() const {
		return static_cast<CursorMode>(glfwGetInputMode(static_cast<GLFWwindow*>(m_Window), GLFW_CURSOR));
	}

	bool Window::GetInputMode(InputMode mode) const {
		return glfwGetInputMode(static_cast<GLFWwindow*>(m_Window), static_cast<int>(mode)) == GLFW_TRUE;
	}

	Vector2i Window::GetRelativePos(const Vector2i &pos) const {
		return {m_Data.X + pos.X, m_Data.Y + pos.Y};
	}

	bool Window::IsVSync() const {
		return m_Data.VSync;
	}

	bool Window::IsVisible() const {
		return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_Window), GLFW_VISIBLE);
	}

	bool Window::IsFullscreen() const {
		return m_Fullscreen;
	}

	void Window::AttentionRequest() {
		glfwRequestWindowAttention(static_cast<GLFWwindow*>(m_Window));
	}

	void Window::ToggleFullscreen() {
		ToggleFullscreen(Monitor::GetPrimary());
	}

	void Window::ToggleFullscreen(Monitor *monitor) {
		ToggleFullscreen(monitor, monitor->GetVideoMode());
	}

	void Window::ToggleFullscreen(Monitor *monitor, const VideoMode *mode) {
		if(glfwGetWindowMonitor(static_cast<GLFWwindow*>(m_Window)) == nullptr) {
			m_Fullscreen = true;
			m_BackupPos  = {m_Data.X, m_Data.Y};
			m_BackupSize = {m_Data.Width, m_Data.Height};

			m_Monitor = monitor;

			glfwSetWindowMonitor(
			                     static_cast<GLFWwindow*>(m_Window),
			                     static_cast<GLFWmonitor*>(monitor->GetNativePointer()),
			                     0,
			                     0,
			                     static_cast<int>(mode->Size.Width),
			                     static_cast<int>(mode->Size.Height),
			                     mode->RefreshRate
			                    );
		}
		else {
			m_Fullscreen = false;
			glfwSetWindowMonitor(
			                     static_cast<GLFWwindow*>(m_Window),
			                     nullptr,
			                     m_BackupPos.X,
			                     m_BackupPos.Y,
			                     static_cast<int>(m_BackupSize.Width),
			                     static_cast<int>(m_BackupSize.Height),
			                     GLFW_DONT_CARE
			                    );

			SetPos(m_BackupPos);
			SetSize(m_BackupSize);
		}
	}

	Cursor* Window::GetCursor() const {
		return m_Cursor.get();
	}

	void Window::SetCursor(Scope<Cursor> &cursor) {
		if(!cursor)
			return;

		if(m_Cursor)
			m_Cursor->Invalidate();

		m_Cursor = std::move(cursor);
		m_Cursor->Attach(this);
	}

	void Window::Invalidate() {
		glfwPostEmptyEvent();
	}

	void Window::Minimalize() {
		glfwIconifyWindow(static_cast<GLFWwindow*>(m_Window));
	}

	void Window::Restore() {
		glfwRestoreWindow(static_cast<GLFWwindow*>(m_Window));
	}

	void Window::Maximalize() {
		glfwMaximizeWindow(static_cast<GLFWwindow*>(m_Window));
	}

	bool Window::IsRawMouseInputSupported() {
		InitializeGlfw();
		return glfwRawMouseMotionSupported() == GLFW_TRUE;
	}

	void Window::InitializeGlfw() {
		if(s_GLFWInitialized)
			return;

		int success = glfwInit();

		ENGINE_ASSERT(success, "Unable to initialize GLFW!");
		if(success == GLFW_TRUE)
			s_GLFWInitialized = true;
		else
			throw std::runtime_error("Unable to initialize GLFW");

		glfwSetErrorCallback(GLFWErrorCallback);
	}

	void Window::Init(const WindowProperties &props) {
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title  = props.Title;

		if(s_GLFWWindowCount == 0)
			InitializeGlfw();

		m_Window = glfwCreateWindow(
		                            static_cast<int>(props.Width),
		                            static_cast<int>(props.Height),
		                            m_Data.Title.c_str(),
		                            nullptr,
		                            nullptr
		                           );

		ENGINE_ASSERT(m_Window);
		if(!m_Window)
			throw std::runtime_error("Unable to create window");

		++s_GLFWWindowCount;

		m_Monitor = nullptr;

		Vector2i pos;
		glfwGetWindowPos(static_cast<GLFWwindow*>(m_Window), &pos.X, &pos.Y);

		m_Data.X = pos.X;
		m_Data.Y = pos.Y;

		SetVSync(true);

		InstallCallbacks();
	}

	void Window::Shutdown() {
		glfwDestroyWindow(static_cast<GLFWwindow*>(m_Window));
		--s_GLFWWindowCount;

		if(s_GLFWWindowCount == 0) {
			glfwTerminate();
			s_GLFWInitialized = false;
		}
	}

	Window::Window(const WindowProperties &props) {
		Init(props);
	}

	void Window::InstallCallbacks() {
		glfwSetWindowFocusCallback(
		                           static_cast<GLFWwindow*>(m_Window),
		                           [](GLFWwindow *window, int focused) {
			                           auto data = GetData(window);

			                           if(focused) {
				                           WindowGainFocusEvent event;
				                           data->EventCallback(event);
			                           }
			                           else {
				                           WindowLostFocusEvent event;
				                           data->EventCallback(event);
			                           }
		                           }
		                          );

		glfwSetWindowPosCallback(
		                         static_cast<GLFWwindow*>(m_Window),
		                         [](GLFWwindow *window, int x, int y) {
			                         auto data = GetData(window);

			                         data->X = x;
			                         data->Y = y;

			                         WindowMovedEvent event(x, y);
			                         data->EventCallback(event);
		                         }
		                        );

		glfwSetWindowSizeCallback(
		                          static_cast<GLFWwindow*>(m_Window),
		                          [](GLFWwindow *window, int width, int height) {
			                          auto data = GetData(window);

			                          data->Width  = static_cast<uint32_t>(width);
			                          data->Height = static_cast<uint32_t>(height);

			                          WindowResizeEvent event(data->Width, data->Height);
			                          data->EventCallback(event);
		                          }
		                         );

		glfwSetWindowCloseCallback(
		                           static_cast<GLFWwindow*>(m_Window),
		                           [](GLFWwindow *window) {
			                           auto data = GetData(window);

			                           WindowCloseEvent event;
			                           data->EventCallback(event);
		                           }
		                          );

		glfwSetKeyCallback(
		                   static_cast<GLFWwindow*>(m_Window),
		                   [](GLFWwindow *window, int key, int scanCode, int action, int mods) {
			                   auto data = GetData(window);

			                   switch(action) {
				                   case GLFW_PRESS:
				                   {
					                   KeyPressedEvent event(key, false);
					                   data->EventCallback(event);
					                   break;
				                   }

				                   case GLFW_RELEASE:
				                   {
					                   KeyReleasedEvent event(key);
					                   data->EventCallback(event);
					                   break;
				                   }

				                   case GLFW_REPEAT:
				                   {
					                   KeyPressedEvent event(key, true);
					                   data->EventCallback(event);
					                   break;
				                   }

				                   default: ENGINE_ASSERT(false);
			                   }
		                   }
		                  );

		glfwSetCharCallback(
		                    static_cast<GLFWwindow*>(m_Window),
		                    [](GLFWwindow *window, unsigned int keyCode) {
			                    auto data = GetData(window);

			                    KeyTypedEvent event(keyCode);
			                    data->EventCallback(event);
		                    }
		                   );

		glfwSetMouseButtonCallback(
		                           static_cast<GLFWwindow*>(m_Window),
		                           [](GLFWwindow *window, int button, int action, int mods) {
			                           auto data = GetData(window);

			                           switch(action) {
				                           case GLFW_PRESS:
				                           {
					                           MouseButtonPressedEvent event(button);
					                           data->EventCallback(event);
					                           break;
				                           }
				                           case GLFW_RELEASE:
				                           {
					                           MouseButtonReleasedEvent event(button);
					                           data->EventCallback(event);
					                           break;
				                           }
				                           default: ENGINE_ASSERT(false);
			                           }
		                           }
		                          );

		glfwSetScrollCallback(
		                      static_cast<GLFWwindow*>(m_Window),
		                      [](GLFWwindow *window, double xOffset, double yOffset) {
			                      auto data = GetData(window);

			                      MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
			                      data->EventCallback(event);
		                      }
		                     );

		glfwSetCursorPosCallback(
		                         static_cast<GLFWwindow*>(m_Window),
		                         [](GLFWwindow *window, double xPos, double yPos) {
			                         auto data = GetData(window);

			                         MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			                         data->EventCallback(event);
		                         }
		                        );
	}

	Window::WindowData* Window::GetData(void *window) {
		return static_cast<WindowData*>(glfwGetWindowUserPointer(static_cast<GLFWwindow*>(window)));
	}
}
