#include "pch.h"
#include "Window.h"

#include "Engine/Renderer/GraphicContext.h"

#include "Engine/Devices/Cursor.h"
#include "Engine/Devices/Monitor.h"

#include <GLFW/glfw3.h>

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine {
	static uint8_t s_GLFWWindowCount = 0;
	static bool s_GLFWInitialized = false;

	static int32_t ToGLFWInputMode(InputMode mode) {
		switch (mode) {
		case InputMode::StickyKeys:
			return GLFW_STICKY_KEYS;
		case InputMode::StickyMouseButtons:
			return GLFW_STICKY_MOUSE_BUTTONS;
		case InputMode::LockKeyModes:
			return GLFW_LOCK_KEY_MODS;
		case InputMode::RawMouseMotion:
			return GLFW_RAW_MOUSE_MOTION;
		}
	}

	static int32_t ToGLFWCursorMode(CursorMode mode) {
		switch (mode) {
		case CursorMode::Normal:
			return GLFW_CURSOR_NORMAL;
		case CursorMode::Hidden:
			return GLFW_CURSOR_HIDDEN;
		case CursorMode::Disabled:
			return GLFW_CURSOR_DISABLED;
		case CursorMode::Captured:
			return GLFW_CURSOR_CAPTURED;
		}
	}

	static CursorMode FromGLFWCursorMode(int mode) {
		switch (mode) {
		case GLFW_CURSOR_NORMAL:
			return CursorMode::Normal;
		case GLFW_CURSOR_HIDDEN:
			return CursorMode::Hidden;
		case GLFW_CURSOR_DISABLED:
			return CursorMode::Disabled;
		case GLFW_CURSOR_CAPTURED:
			return CursorMode::Captured;
		default:
			return CursorMode::Normal;
		}
	}

	static void GLFWErrorCallback(int error, const char* desc) {
		LOG_ENGINE_ERROR("GLFW Error ({0}): {1}", error, desc);
	}

	Scope<Window> Window::Create(const WindowProperties& props) {
		return Scope<Window>(new Window(props));
	}

	Window::~Window() {
		m_Context = nullptr;
		Shutdown();
		if (m_Cursor)
			m_Cursor->Invalidate();
	}

	void Window::OnUpdate() {
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::SetInputMode(InputMode mode, bool enabled) {
		glfwSetInputMode(GetNativeHandle<GLFWwindow>(), ToGLFWInputMode(mode), enabled ? GLFW_TRUE : GLFW_FALSE);
	}

	void Window::SetCursorMode(CursorMode mode) {
		glfwSetInputMode(GetNativeHandle<GLFWwindow>(), GLFW_CURSOR, ToGLFWCursorMode(mode));
	}

	void Window::SetSize(const Vector2u& size) {
		SetSize(size.Width, size.Height);
	}

	void Window::SetSize(uint32_t width, uint32_t height) {
		glfwSetWindowSize(GetNativeHandle<GLFWwindow>(), static_cast<int>(width), static_cast<int>(height));

		m_Data.Width = width;
		m_Data.Height = height;
	}

	void Window::SetPos(int32_t x, int32_t y) {
		glfwSetWindowPos(GetNativeHandle<GLFWwindow>(), x, y);

		m_Data.X = x;
		m_Data.Y = y;
	}

	void Window::SetPos(const Vector2i& pos) {
		SetPos(pos.X, pos.Y);
	}

	void Window::SetVSync(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	void Window::SetTitle(std::string title) {
		glfwSetWindowTitle(GetNativeHandle<GLFWwindow>(), title.c_str());
		m_Data.Title = std::move(title);
	}

	void Window::Visible(bool visible) {
		if (visible)
			glfwShowWindow(GetNativeHandle<GLFWwindow>());
		else
			glfwHideWindow(GetNativeHandle<GLFWwindow>());
	}

	CursorMode Window::GetCursorMode() const {
		return FromGLFWCursorMode(glfwGetInputMode(GetNativeHandle<GLFWwindow>(), GLFW_CURSOR));
	}

	bool Window::GetInputMode(InputMode mode) const {
		return glfwGetInputMode(GetNativeHandle<GLFWwindow>(), ToGLFWInputMode(mode)) == GLFW_TRUE;
	}

	Vector2i Window::GetRelativePos(const Vector2i& pos) const {
		return {m_Data.X + pos.X, m_Data.Y + pos.Y};
	}

	bool Window::IsVSync() const {
		return m_Data.VSync;
	}

	bool Window::IsVisible() const {
		return glfwGetWindowAttrib(GetNativeHandle<GLFWwindow>(), GLFW_VISIBLE);
	}

	bool Window::IsFullscreen() const {
		return m_Fullscreen;
	}

	void Window::AttentionRequest() const {
		glfwRequestWindowAttention(GetNativeHandle<GLFWwindow>());
	}

	void Window::ToggleFullscreen(Monitor* monitor) {
		if (!monitor)
			monitor = Monitor::GetPrimary();

		ToggleFullscreen(monitor, monitor->GetVideoMode());
	}

	void Window::ToggleFullscreen(Monitor* monitor, const VideoMode* mode) {
		if (glfwGetWindowMonitor(GetNativeHandle<GLFWwindow>()) == nullptr) {
			m_Fullscreen = true;
			m_BackupPos = {m_Data.X, m_Data.Y};
			m_BackupSize = {m_Data.Width, m_Data.Height};

			m_Monitor = monitor;

			glfwSetWindowMonitor(
				GetNativeHandle<GLFWwindow>(),
				monitor->GetNativeHandle<GLFWmonitor>(),
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
				GetNativeHandle<GLFWwindow>(),
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

	void Window::SetCursor(Scope<Cursor> cursor) {
		if (!cursor)
			return;

		if (m_Cursor)
			m_Cursor->Invalidate();

		m_Cursor = std::move(cursor);
		m_Cursor->Attach(this);
		m_Cursor->Apply();
	}

	void Window::Invalidate() {
		glfwPostEmptyEvent();
	}

	void Window::Minimalize() {
		glfwIconifyWindow(GetNativeHandle<GLFWwindow>());
	}

	void Window::Restore() {
		glfwRestoreWindow(GetNativeHandle<GLFWwindow>());
	}

	void Window::Maximize() {
		glfwMaximizeWindow(GetNativeHandle<GLFWwindow>());
	}

	bool Window::IsRawMouseInputSupported() {
		InitializeGlfw();
		return glfwRawMouseMotionSupported() == GLFW_TRUE;
	}

	void Window::InitializeGlfw() {
		if (s_GLFWInitialized)
			return;

		int success = glfwInit();

		ENGINE_ASSERT(success, "Unable to initialize GLFW!");
		if (success == GLFW_TRUE)
			s_GLFWInitialized = true;
		else
			throw std::runtime_error("Unable to initialize GLFW");

		glfwSetErrorCallback(GLFWErrorCallback);
	}

	void Window::Init(const WindowProperties& props) {
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;

		m_Window = Create(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title, nullptr,
		                  nullptr);

		m_Monitor = nullptr;
		m_Context = GraphicContext::Create(this);

		Vector2i pos;
		glfwGetWindowPos(GetNativeHandle<GLFWwindow>(), &pos.X, &pos.Y);

		m_Data.X = pos.X;
		m_Data.Y = pos.Y;

		glfwSetWindowUserPointer(GetNativeHandle<GLFWwindow>(), &m_Data);
		SetVSync(true);

		InstallCallbacks();
	}

	void Window::Shutdown() {
		glfwDestroyWindow(GetNativeHandle<GLFWwindow>());
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0) {
			glfwTerminate();
			s_GLFWInitialized = false;
		}
	}

	void* Window::Create(int width, int height, std::string_view name, void* monitor, void* share) {
		if (s_GLFWWindowCount == 0)
			InitializeGlfw();

		auto window = glfwCreateWindow(width, height, name.data(), static_cast<GLFWmonitor*>(monitor),
		                               static_cast<GLFWwindow*>(share));

		ENGINE_ASSERT(window);
		if (!window)
			throw std::runtime_error("Unable to create window");

		++s_GLFWWindowCount;

		return window;
	}

	Window::Window(const WindowProperties& props) : m_Cursor(nullptr), m_Context(nullptr) {
		Init(props);
	}

	void Window::InstallCallbacks() {
		glfwSetWindowFocusCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, int focused) {
				auto data = GetData(window);

				if (focused) {
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
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, int x, int y) {
				auto data = GetData(window);

				data->X = x;
				data->Y = y;

				WindowMovedEvent event(x, y);
				data->EventCallback(event);
			}
		);

		glfwSetWindowSizeCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, int width, int height) {
				auto data = GetData(window);

				data->Width = static_cast<uint32_t>(width);
				data->Height = static_cast<uint32_t>(height);

				WindowResizeEvent event(data->Width, data->Height);
				data->EventCallback(event);
			}
		);

		glfwSetWindowCloseCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window) {
				auto data = GetData(window);

				WindowCloseEvent event;
				data->EventCallback(event);
			}
		);

		glfwSetKeyCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, int key, int scanCode, int action, int mods) {
				auto data = GetData(window);

				switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(static_cast<KeyCode>(key), false);
					data->EventCallback(event);
					break;
				}

				case GLFW_RELEASE: {
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data->EventCallback(event);
					break;
				}

				case GLFW_REPEAT: {
					KeyPressedEvent event(static_cast<KeyCode>(key), true);
					data->EventCallback(event);
					break;
				}

				default: ENGINE_ASSERT(false);
				}
			}
		);

		glfwSetCharCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, unsigned int keyCode) {
				auto data = GetData(window);

				KeyTypedEvent event(static_cast<KeyCode>(keyCode));
				data->EventCallback(event);
			}
		);

		glfwSetMouseButtonCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, int button, int action, int mods) {
				auto data = GetData(window);

				switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data->EventCallback(event);
					break;
				}
				default: ENGINE_ASSERT(false);
				}
			}
		);

		glfwSetScrollCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, double xOffset, double yOffset) {
				auto data = GetData(window);

				MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
				data->EventCallback(event);
			}
		);

		glfwSetCursorPosCallback(
			GetNativeHandle<GLFWwindow>(),
			[](GLFWwindow* window, double xPos, double yPos) {
				auto data = GetData(window);

				MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
				data->EventCallback(event);
			}
		);
	}

	Window::WindowData* Window::GetData(void* window) {
		return static_cast<WindowData*>(glfwGetWindowUserPointer(static_cast<GLFWwindow*>(window)));
	}
}
