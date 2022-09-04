#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Utils/Utils.h"

#include <functional>
#include <unordered_set>

namespace Game
{
	class KeyReleasedEvent;
	class KeyPressedEvent;

	class Shortcut
	{
	public:
		using ShortcutFunction = std::function<void()>;
		
	private:
		size_t m_Keys = 0;
		size_t m_Pressed = 0;
		bool m_Detected = false;
		ShortcutFunction m_Function;

		std::unordered_set<KeyCode> m_KeyCodes;
	public:
		template<typename ...Args>
		explicit Shortcut(const ShortcutFunction& function, Args&&... args) : m_Function(function)
		{
			m_KeyCodes.reserve(TypeListLengthV<Args...>);
			Register(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void Register(KeyCode code, Args&&... args)
		{
			RegisterPriv(code);
			if constexpr (TypeListLengthV<Args...> > 0) Register(std::forward<Args>(args)...); 
		}

		void OnEvent(Event &event);

	private:
		bool OnKeyPress(KeyPressedEvent &event);
		bool OnKeyRelease(KeyReleasedEvent &event);
		
		void RegisterPriv(KeyCode code);

		// void ProcessKeyPressed(KeyCode code);
		// void ProcessKeyReleased(KeyCode code);
	};
}
