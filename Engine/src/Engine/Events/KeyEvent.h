#pragma once

#include "Engine/Events/Event.h"
#include "Engine/Core/KeyCodes.h"

namespace Game
{
	class KeyEvent: public Event
	{
	protected:
		KeyCode m_KeyCode;
		explicit KeyEvent(const KeyCode keyCode) : m_KeyCode(keyCode) {}

	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};

	class KeyPressedEvent: public KeyEvent
	{
		uint16_t m_RepeatCount;

	public:
		KeyPressedEvent(const KeyCode keyCode, const uint16_t repeatCount) : KeyEvent(keyCode),
		                                                                     m_RepeatCount(repeatCount) {}

		uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent: public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keyCode) : KeyEvent(keyCode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
