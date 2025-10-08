#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Flags.h"

#include <string>
#include <string_view>
#include <functional>

namespace Engine {
	struct VideoMode;

	class Event;
	class Cursor;
	class Monitor;
	class GraphicContext;

	enum class WindowStateFlags : uint8_t {
		None = 0,
		Visible = 1 << 0,
		Focused = 1 << 1,
		Minimized = 1 << 2,
		Maximized = 1 << 3,
		Fullscreen = 1 << 4,
		Vsync = 1 << 5
	};

	enum class InputMode {
		StickyKeys,
		StickyMouseButtons,
		LockKeyModes,
		RawMouseMotion
	};

	enum class CursorMode {
		Normal,
		Hidden,
		Disabled,
		Captured
	};

	struct WindowProperties {
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		Flags<WindowStateFlags> InitialFlags { WindowStateFlags::None };

		WindowProperties(std::string title, uint32_t width, uint32_t height, WindowStateFlags flags = WindowStateFlags::None) : Title(std::move(title)), Width(width), Height(height), InitialFlags(flags) {}
		WindowProperties(std::string title, const Vector2u &size, WindowStateFlags flags = WindowStateFlags::None) : Title(std::move(title)), Width(size.Width), Height(size.Height), InitialFlags(flags) {}
	};

	class Window {
		friend class Monitor;
		friend class GraphicContext;

	public:
		using EventCallbackFunction = std::move_only_function<void(Event &)>;

		[[nodiscard]] static Scope<Window> Create(const WindowProperties &props);
		virtual ~Window();

		void OnUpdate();

		void SetInputMode(InputMode mode, bool enabled = true);
		void SetCursorMode(CursorMode mode);

		void SetSize(const Vector2u &size);
		void SetSize(uint32_t width, uint32_t height);

		void SetPos(int32_t x, int32_t y);
		void SetPos(const Vector2i &pos);

		void SetEventCallback(EventCallbackFunction &&callback) { m_Data.EventCallback = std::move(callback); }
		void SetVSync(bool enabled = true);

		void SetTitle(std::string_view title);

		void Visible(bool visible = true);

		template<typename T>
		[[nodiscard]] T* GetNativeHandle() const { return static_cast<T*>(m_Window); }

		[[nodiscard]] void* GetNativeHandle() const { return m_Window; }

		[[nodiscard]] std::string_view GetTitle() const { return m_Data.Title; }

		[[nodiscard]] uint32_t GetWidth() const { return m_Data.Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Data.Height; }
		[[nodiscard]] Vector2u GetSize() const { return Vector2u{m_Data.Width, m_Data.Height}; }

		[[nodiscard]] int32_t GetX() const { return m_Data.X; }
		[[nodiscard]] int32_t GetY() const { return m_Data.Y; }
		[[nodiscard]] Vector2i GetPos() const { return {m_Data.X, m_Data.Y}; }

		[[nodiscard]] CursorMode GetCursorMode() const;
		[[nodiscard]] bool GetInputMode(InputMode mode) const;

		[[nodiscard]] Vector2i GetRelativePos(const Vector2i &pos) const;

		[[nodiscard]] Monitor *GetMonitor() const { return m_Monitor; }

		[[nodiscard]] bool IsVSync() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Vsync); }
		[[nodiscard]] bool IsVisible() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Visible); }
		[[nodiscard]] bool IsFullscreen() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Fullscreen); }

		[[nodiscard]] float GetContentScaleWidth() const noexcept { return m_Data.ContentScaleX; }
		[[nodiscard]] float GetContentScaleHeight() const noexcept { return m_Data.ContentScaleY; }
		[[nodiscard]] Vector2f GetContentScale() const noexcept { return { m_Data.ContentScaleX, m_Data.ContentScaleY }; }

		[[nodiscard]] uint32_t GetFramebufferWidth() const noexcept { return m_Data.FramebufferWidth; }
		[[nodiscard]] uint32_t GetFramebufferHeight() const noexcept { return m_Data.FramebufferHeight; }
		[[nodiscard]] Vector2u GetFramebufferSize() const noexcept { return { m_Data.FramebufferWidth, m_Data.FramebufferHeight }; }

		[[nodiscard]] bool IsFocused() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Focused); }
		[[nodiscard]] bool IsMinimized() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Minimized); }
		[[nodiscard]] bool IsMaximized() const noexcept { return m_Data.State.HasAny(WindowStateFlags::Maximized); }

		[[nodiscard]] bool IsOpen() const noexcept { return m_Window != nullptr; }
		[[nodiscard]] GraphicContext* GetContext() const { return m_Context.get(); }

		void AttentionRequest() const;

		void ToggleFullscreen(Monitor *monitor = nullptr);
		void ToggleFullscreen(Monitor *monitor, const VideoMode *mode);

		[[nodiscard]] Cursor* GetCursor() const;
		void SetCursor(Scope<Cursor> cursor);

		void Invalidate();

		void Minimize();
		void Restore();
		void Maximize();

		[[nodiscard]] static bool IsRawMouseInputSupported();
	protected:
		static void InitializeGlfw();

		void Init(const WindowProperties &props);
		void Shutdown();

		static void* Create(int width, int height, const std::string& name, void* monitor, void* share);
	private:
		explicit Window(const WindowProperties &props);

		void InstallCallbacks();

		struct WindowData {
			std::string Title = {};

			uint32_t Width = 1;
			uint32_t Height = 1;

			int32_t X = 0;
			int32_t Y = 0;

			uint32_t FramebufferWidth = 0;
			uint32_t FramebufferHeight = 0;

			float ContentScaleX = 1.f;
			float ContentScaleY = 1.f;

			Flags<WindowStateFlags> State{ WindowStateFlags::None };
			EventCallbackFunction EventCallback = {};
		};

		struct BackupData {
			Vector2i Pos;
			Vector2u Size;
		};

		static WindowData *GetData(void *window);

		WindowData m_Data;
		BackupData m_Backup;

		void *m_Window;

		Monitor *m_Monitor;

		Scope<Cursor> m_Cursor;
		Scope<GraphicContext> m_Context;
	};

	constexpr WindowStateFlags operator|(WindowStateFlags a, WindowStateFlags b) noexcept {
		return static_cast<WindowStateFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	constexpr WindowStateFlags operator&(WindowStateFlags a, WindowStateFlags b) noexcept {
		return static_cast<WindowStateFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	constexpr WindowStateFlags& operator|=(WindowStateFlags& a, WindowStateFlags b) noexcept {
		a = a | b;
		return a;
	}
}
