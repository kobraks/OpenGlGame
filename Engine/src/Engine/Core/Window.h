#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include <string>
#include <string_view>
#include <functional>

namespace Engine {
	struct VideoMode;

	class Event;
	class Cursor;
	class Context;
	class Monitor;

	enum class InputMode {
		//From GLFW
		StickyKeys = 0x00033002,
		StickyMouseButtons = 0x00033003,
		LockKeyModes = 0x00033004,
		RawMouseMotion = 0x00033005
	};

	enum class CursorMode {
		//From GLFW
		Normal = 0x00034001,
		Hidden  = 0x00034002,
		Disabled = 0x00034003
	};

	struct WindowProperties {
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProperties(std::string title, uint32_t width, uint32_t height) : Title(std::move(title)), Width(width), Height(height) {}
	};

	class Window {
		friend class Monitor;

	public:
		using EventCallbackFunction = std::function<void(Event &)>;

		static Scope<Window> Create(const WindowProperties &props);
		virtual ~Window();

		void OnUpdate();

		void SetInputMode(InputMode mode, bool enabled = true);
		void SetCursorMode(CursorMode mode);

		void SetSize(const Vector2u &size);
		void SetSize(uint32_t width, uint32_t height);

		void SetPos(int32_t x, int32_t y);
		void SetPos(const Vector2i &pos);

		void SetEventCallback(const EventCallbackFunction &callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled = true);

		void SetTitle(std::string title);

		void Visible(bool visible = true);

		void* GetNativeWindow() const { return m_Window; }

		std::string_view GetTitle() const { return m_Data.Title; }

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		Vector2u GetSize() const { return Vector2u{m_Data.Height, m_Data.Height}; }

		int32_t GetX() const { return m_Data.X; }
		int32_t GetY() const { return m_Data.Y; }
		Vector2i GetPos() const { return {m_Data.X, m_Data.Y}; }

		CursorMode GetCursorMode() const;
		bool GetInputMode(InputMode mode) const;

		Vector2i GetRelativePos(const Vector2i &pos) const;

		Monitor *GetMonitor() const { return m_Monitor; }

		bool IsVSync() const;
		bool IsVisible() const;
		bool IsFullscreen() const;

		void AttentionRequest();

		void ToggleFullscreen();
		void ToggleFullscreen(Monitor *monitor);
		void ToggleFullscreen(Monitor *monitor, const VideoMode *mode);

		Cursor* GetCursor() const;
		void SetCursor(Scope<Cursor> &cursor);

		void Invalidate();

		void Minimalize();
		void Restore();
		void Maximalize();

		static bool IsRawMouseInputSupported();
	protected:
		static void InitializeGlfw();

		void Init(const WindowProperties &props);
		void Shutdown();

	private:
		explicit Window(const WindowProperties &props);

		void InstallCallbacks();

		struct WindowData {
			std::string Title;

			uint32_t Width;
			uint32_t Height;

			int32_t X;
			int32_t Y;

			bool VSync = false;
			EventCallbackFunction EventCallback;
		};

		static WindowData *GetData(void *window);

		Vector2i m_BackupPos;
		Vector2u m_BackupSize;

		bool m_Fullscreen = false;

		WindowData m_Data;

		void *m_Window;

		Monitor *m_Monitor;

		Scope<Cursor> m_Cursor = nullptr;
		Scope<Context> m_Context = nullptr;
	};
}