#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/MouseButton.h"
#include "Engine/Core/Vector2.h"

#include "Engine/Events/Event.h"

namespace Engine{
	class MouseMovedEvent: public Event {
	public:
		MouseMovedEvent(const float x, const float y) : m_Pos{x, y} {}

		float GetX() const { return m_Pos.X; }
		float GetY() const { return m_Pos.Y; }

		Vector2f GetPos() const { return m_Pos; }

		std::string ToString() const override {
			return fmt::format("MouseMovedEvent: {}, {}", m_Pos.X, m_Pos.Y);
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		Vector2f m_Pos;
	};

	class MouseScrolledEvent: public Event {
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset) : m_Offset{xOffset, yOffset} {}

		float GetXOffset() const { return m_Offset.X; }
		float GetYOffset() const { return m_Offset.Y; }

		Vector2f GetOffset() const { return m_Offset; }

		std::string ToString() const override {
			return fmt::format("MouseScrolledEvent: {}, {}", m_Offset.X, m_Offset.Y);
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		Vector2f m_Offset;
	};

	class MouseButtonEvent: public Event {
	public:
		MouseButtonCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

	protected:
		MouseButtonEvent(const MouseButtonCode button) : m_Button(button) {}

		MouseButtonCode m_Button;
	};

	class MouseButtonPressedEvent: public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(const MouseButtonCode button): MouseButtonEvent(button) {}

		std::string ToString() const override {
			return fmt::format("MouseButtonPressedEvent: {}", static_cast<uint16_t>(m_Button));
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent: public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(const MouseButtonCode button): MouseButtonEvent(button) {}

		std::string ToString() const override {
			return fmt::format("MouseButtonReleasedEvent: {}", static_cast<uint16_t>(m_Button));
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}