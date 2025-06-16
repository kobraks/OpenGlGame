#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

#include "Engine/Events/KeyEvent.h"

namespace Engine {
	class LogStorage;
	class LogPanel;
	class LogSink;

	class LogLayer : public Layer {
	public:
		LogLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		bool OnKeyPressEvent(KeyPressedEvent& event);

	private:
		Ref<LogStorage> m_Storage;
		Ref<LogPanel> m_Panel;
		Ref<LogSink> m_Sink;
	};
}
