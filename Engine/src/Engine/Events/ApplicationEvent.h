#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include "Engine/Events/Event.h"

namespace Engine {
	class WindowFocusEvent: public Event {
	public:
		explicit WindowFocusEvent(bool focused) : m_Focused(focused) {}

		bool IsFocused() const { return m_Focused; }

		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	private:
		bool m_Focused{};
	};

	class WindowLostFocusEvent: public WindowFocusEvent {
	public:
		WindowLostFocusEvent() : WindowFocusEvent(false) {}

		EVENT_CLASS_TYPE(WindowLostFocus)
	};

	class WindowGainFocusEvent: public WindowFocusEvent {
	public:
		WindowGainFocusEvent() : WindowFocusEvent(true) {}

		EVENT_CLASS_TYPE(WindowFocus)
	};

	class WindowMovedEvent: public Event {
	public:
		WindowMovedEvent(int32_t x, int32_t y) : m_Pos{x, y} {}

		int32_t GetX() const { return m_Pos.X; }
		int32_t GetY() const { return m_Pos.Y; }

		Vector2i GetPos() const { return m_Pos; }

		std::string ToString() const override {
			return fmt::format("WindowMovedEvent: {}, {}", m_Pos.X, m_Pos.Y);
		}

		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		Vector2i m_Pos{};
	};

	class WindowResizeEvent: public Event {
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Size{width, height} {}

		uint32_t GetWidth() const { return m_Size.Width; }
		uint32_t GetHeight() const { return m_Size.Height; }

		Vector2u GetSize() const { return m_Size; }

		std::string ToString() const override {
			return fmt::format("WindowResizeEvent: {}, {}", m_Size.Width, m_Size.Height);
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		Vector2u m_Size;
	};

	class WindowCloseEvent: public Event {
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppTickEvent: public Event {
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent: public Event {
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent: public Event {
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
