#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/Context.h"
#include "Engine/Devices/Monitor.h"
#include "Engine/Devices/Cursor.h"

#include <String>
#include <vector>
#include <functional>
#include <GLFW/glfw3.h>
#include <string_view>

namespace Game
{
	enum class InputMode
	{
		StickyKeys = GLFW_STICKY_KEYS,
		StickyMouseButtons = GLFW_STICKY_MOUSE_BUTTONS,
		LockKeyModes = GLFW_LOCK_KEY_MODS,
		RawMouseMotion = GLFW_RAW_MOUSE_MOTION
	};

	enum class CursorMode
	{
		Normal = GLFW_CURSOR_NORMAL,
		Hidden = GLFW_CURSOR_HIDDEN,
		Disabled = GLFW_CURSOR_DISABLED
	};
	
	struct WindowProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProperties(const std::string title, uint32_t width, uint32_t height) : Title(title),
		                                                                             Width(width),
		                                                                             Height(height) { }
	};

	class Window
	{
		friend class Monitor;

	public:
		using EventCallbackFunction = std::function<void(Event &)>;

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width = 0;
			uint32_t Height = 0;

			uint32_t X = 0;
			uint32_t Y = 0;

			bool VSync = false;
			EventCallbackFunction EventCallback;
		};

		Vector2u m_BackupPosition;
		Vector2u m_BackupSize;

		bool m_Fullscreen = false;

		GLFWwindow *m_Window;
		GLFWwindow *m_WindowContext;
		Scope<Context> m_Context;

		WindowData m_Data;

		Scope<Monitor> m_Monitor = nullptr;
		Scope<Cursor> m_Cursor   = nullptr;
	public:
		explicit Window(const WindowProperties &properties);
		virtual ~Window();

		void OnUpdate();

		void SetInputMode(bool enabled, InputMode mode);
		bool GetInputMode(InputMode mode) const;
		
		void SetCursorMode(CursorMode mode);
		bool GetCursorMode(CursorMode mode);
		CursorMode GetCursorMode();
		
		static bool IsRawMouseInputSupported();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		std::string_view GetTitle() const { return m_Data.Title; }

		Vector2<uint32_t> GetSize() const { return Vector2u{GetWidth(), GetHeight()}; }
		void SetSize(Vector2u size);
		void SetSize(uint32_t width, uint32_t height);

		void SetEventCallback(const EventCallbackFunction &callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const { return m_Data.VSync; };

		void* GetNativeWindow() const { return m_Window; };

		OpenGlFunctions GetFunctions() const;

		void SetPosition(uint32_t x, uint32_t y);
		void SetPosition(const Vector2u &position) { return SetPosition(position.X, position.Y); }

		Vector2u GetPosition() const { return {m_Data.X, m_Data.Y}; }

		void SetTitle(const std::string &title);

		void Visible(bool visible);

		bool IsVisible() const;

		void AttentionRequest();

		Vector2u GetRelativePosition(const Vector2u &position) const;

		Monitor GetMonitor() const { return *m_Monitor; }
		VideoMode GetVideoMode() const { return m_Monitor->GetVideoMode(); }

		void ToggleFullscreen();
		void ToggleFullscreen(const Monitor &monitor);
		void ToggleFullscreen(const Monitor &monitor, const VideoMode &mode);

		Cursor* GetCursor() const;
		void SetCursor(std::unique_ptr<Cursor> &&cursor);

		void Invalidate();

		bool IsFullscreen() const;

		void Minimalize();
		void Restore();
		void Maximalize();
	protected:
		static void InitializeGlfw();

		void Init(const WindowProperties &props);
		void Shutdown();
	};
}
