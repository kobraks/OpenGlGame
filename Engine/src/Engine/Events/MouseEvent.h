#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Devices/Mouse.h"

namespace Game
{
	class MouseMovedEvent: public Event
	{
		float m_MouseX, m_MouseY;

	public:
		MouseMovedEvent(const float x, const float y) : m_MouseX(x),
		                                                m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class MouseScrolledEvent: public Event
	{
		float m_XOffset, m_YOffset;

	public:
		MouseScrolledEvent(const float &xOffset, const float &yOffset) : m_XOffset(xOffset),
		                                                                 m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class MouseButtonEvent: public Event
	{
	protected:
		Mouse::CodeType m_Button;

		MouseButtonEvent(const Mouse::CodeType button) : m_Button(button) {}
	public:
		Mouse::CodeType GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	};

	class MouseButtonPressedEvent: public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const Mouse::CodeType button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const Mouse::CodeType button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}
