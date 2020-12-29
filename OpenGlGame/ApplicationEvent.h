#pragma once

#include "Event.h"
#include "Vector2.h"

namespace Game
{
	class WindowFocusEvent : public Event
	{
		bool m_Focused;

	public:
		WindowFocusEvent(bool focused) : m_Focused(focused) {}

		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class WindowLostFocusEvent : public  WindowFocusEvent
	{
	public:
		WindowLostFocusEvent() : WindowFocusEvent(false) {}

		EVENT_CLASS_TYPE(WindowLostFocus);
	};

	class WindowGainFocusEvent : public WindowFocusEvent
	{
	public:
		WindowGainFocusEvent() : WindowFocusEvent(true) {}

		EVENT_CLASS_TYPE(WindowFocus);
	};
	
	class WindowMovedEvent : public Event
	{
		uint32_t m_X, m_Y;
	public:
		WindowMovedEvent(uint32_t x, uint32_t y) : m_X(x), m_Y(y) {  }

		uint32_t GetXPos() const { return m_X; };
		uint32_t GetYPos() const { return m_Y; };

		Vector2u GetPosition() const { return Vector2u(m_X, m_Y); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_X << ", " << m_Y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowMoved);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};
	
	class WindowResizeEvent : public Event
	{
		uint32_t m_Width, m_Height;
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
		{}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const {return m_Height; }

		Vector2u GetSize() const { return Vector2u(m_Width, m_Height); }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};
}