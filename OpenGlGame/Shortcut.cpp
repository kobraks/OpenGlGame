#include "pch.h"
#include "Shortcut.h"
#include "KeyEvent.h"

namespace Game
{
	void Shortcut::OnEvent(const Event &event)
	{
		if(KeyPressedEvent::GetStaticType() == event.GetEventType())
		{
			const KeyPressedEvent *pressedEvent = dynamic_cast<const KeyPressedEvent*>(&event);

			if(pressedEvent)
			{
				ProcessKeyPressed(pressedEvent->GetKeyCode());
			}
		}
		else if(KeyReleasedEvent::GetStaticType() == event.GetEventType())
		{
			const KeyReleasedEvent *pressedEvent = dynamic_cast<const KeyReleasedEvent*>(&event);

			if(pressedEvent)
			{
				ProcessKeyReleased(pressedEvent->GetKeyCode());
			}
		}

		if (m_Pressed == 0)
			m_Detected = false;

		if(m_Pressed == m_KeyCodes.size() && !m_Detected)
		{
			m_Detected = true;
			m_Function();
		}
	}

	void Shortcut::RegisterPriv(KeyCode code)
	{
		m_Keys++;
		m_KeyCodes.emplace(std::make_pair(code, false));
	}

	void Shortcut::ProcessKeyPressed(KeyCode code)
	{
		auto it = m_KeyCodes.find(code);
		if(it != m_KeyCodes.find(code))
		{
			m_KeyCodes.at(code) = true;
			m_Pressed++;
		}
	}

	void Shortcut::ProcessKeyReleased(KeyCode code)
	{
		auto it = m_KeyCodes.find(code);
		if(it != m_KeyCodes.find(code))
		{
			m_KeyCodes.at(code) = false;
			m_Pressed--;
		}
	}
}
