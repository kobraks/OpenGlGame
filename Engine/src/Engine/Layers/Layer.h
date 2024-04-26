#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Time.h"

#include "Engine/Events/Event.h"

#include <string>

namespace Engine {
	class Layer {
	public:
		explicit Layer(std::string name = "Layer") : m_DebugName(std::move(name)) {}
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnConstUpdate(const Time &timeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event &event) {}

		std::string_view GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}