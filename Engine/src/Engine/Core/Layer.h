#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Time.h"
#include "Engine/Events/Event.h"

#include <string>

namespace Game
{
	class Layer
	{
		std::string m_DebugName;
	public:
		explicit Layer(const std::string &name = "Layer") : m_DebugName(name)
		{
		}

		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnConstUpdate(const Time& timeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_DebugName; }
	};
}