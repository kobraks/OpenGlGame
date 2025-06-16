#include "pch.h"
#include "LogLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/Events/KeyEvent.h"

#include "Engine/Devices/Keyboard.h"

#include "Engine/Logging/LogSink.h"
#include "Engine/Logging/LogStorage.h"
#include "Engine/ImGui/LogPanel.h"

namespace Engine {
	LogLayer::LogLayer() : Layer("LogLayer") {
		m_Storage = MakeRef<LogStorage>();
		m_Sink = MakeRef<LogSink>(m_Storage);
		m_Panel = MakeRef<LogPanel>(m_Storage);

		Log::GetApplicationLogger()->sinks().push_back(m_Sink);
		Log::GetEngineLogger()->sinks().push_back(m_Sink);
		Log::GetGLLogger()->sinks().push_back(m_Sink);
		Log::GetScriptLogger()->sinks().push_back(m_Sink);
	}

	void LogLayer::OnAttach() {
	}

	void LogLayer::OnImGuiRender() {
		m_Panel->Render();
	}

	void LogLayer::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(LogLayer::OnKeyPressEvent));
	}

	bool LogLayer::OnKeyPressEvent(KeyPressedEvent& event) {
		if (event.IsRepeat())
			return false;

		if (event.GetKeyCode() == Key::F2) {
			m_Panel->SetVisible(!m_Panel->IsVisible());
		}

		return false;
	}
}
