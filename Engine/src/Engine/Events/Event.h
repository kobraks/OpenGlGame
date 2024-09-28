#pragma once
#include "Engine/Core/Base.h"

#include <functional>
#include <sstream>
#include <string_view>

#include <fmt/format.h>

namespace Engine {
	enum class EventType {
		None = 0,

		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		AppTick,
		AppUpdate,
		AppRender,

		KeyPressed,
		KeyReleased,
		KeyTyped,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual std::string_view GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event {
	public:
		virtual ~Event() {}

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual std::string_view GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		virtual std::string ToString() const { return std::string(GetName()); }

		bool IsInCategory(EventCategory cat) const {
			return GetCategoryFlags() & cat;
		}
	};

	class EventDispatcher {
	public:
		EventDispatcher(Event &event) : m_Event(event) {}

		template <typename T, typename F>
		bool Dispatch(const F &func) {
			if(m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event &m_Event;
	};
}

template <class Out>
Out operator<<(Out &out, const Engine::Event &event) {
	return Out << event.ToString();
}

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<Engine::Event, T>, char>>: formatter<std::string> {
	auto format(const Engine::Event &e, format_context &ctx) const {
		return formatter<std::string>::format(e.ToString(), ctx);
	}
};
