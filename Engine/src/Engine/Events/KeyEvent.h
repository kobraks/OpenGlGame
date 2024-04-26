#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/KeyCode.h"

#include "Engine/Events/Event.h"

namespace Engine {
	class KeyEvent: public Event {
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		explicit KeyEvent(const KeyCode keyCode): m_KeyCode(keyCode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent: public KeyEvent {
	public:
		explicit KeyPressedEvent(const KeyCode keyCode, bool isRepeat = false): KeyEvent(keyCode), m_IsRepeat{isRepeat} {}

		bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override {
			return fmt::format("KeyPressedEvent: {} (repeat = {} )", static_cast<int16_t>(m_KeyCode), m_IsRepeat);
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent: public KeyEvent {
	public:
		explicit KeyReleasedEvent(const KeyCode keyCode): KeyEvent(keyCode) {}

		std::string ToString() const override {
			return fmt::format("KeyTypedEvent: {}", static_cast<int16_t>(m_KeyCode));
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent: public KeyEvent {
	public:
		explicit KeyTypedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

		std::string ToString() const override {
			return fmt::format("KeyTypedEvent: {}", static_cast<int16_t>(m_KeyCode));
		}

		EVENT_CLASS_TYPE(KeyTyped);
	};
}
