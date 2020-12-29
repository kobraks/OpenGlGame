#pragma once

#include "Time.h"
#include "Event.h"

namespace Game
{
	class Layer
	{
		std::string m_Name;
	public:
		
		explicit Layer(const std::string &name = "Layer") : m_Name(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnConstUpdate(const Time& timeStep){}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_Name; }
 	};
}
