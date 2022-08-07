#include "pch.h"
#include "Shortcut.h"
#include "Engine/Events/KeyEvent.h"

namespace Game
{
	void Shortcut::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Shortcut::OnKeyPress));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Shortcut::OnKeyRelease));

		if(m_Pressed == m_KeyCodes.size() && !m_Detected)
		{
			m_Detected = true;
			m_Function();
		}
	}

	bool Shortcut::OnKeyPress(KeyPressedEvent &event)
	{
		if (!event.IsRepeat())
		{
			const auto code = event.GetKeyCode();

			if(m_KeyCodes.contains(code))
				++m_Pressed;
		}
		
		return false;
	}

	bool Shortcut::OnKeyRelease(KeyReleasedEvent &event)
	{
		const auto code = event.GetKeyCode();

		if(m_KeyCodes.contains(code))
		{
			--m_Pressed;
			m_Detected = false;
		}
		
		return false;
	}

	void Shortcut::RegisterPriv(KeyCode code)
	{
		m_Keys++;
		m_KeyCodes.emplace(code);
	}
}
