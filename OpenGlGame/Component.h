#pragma once
#include <string>
#include <utility>

#include "Types.h"

namespace Game
{
	class Component
	{
		std::string m_Name;
		
	public:
		Component(std::string name) : m_Name(std::move(name)) {}
		virtual ~Component() {}

		virtual Pointer<Component> Clone() = 0;

		const std::string& Name() const { return m_Name; }
	};
}