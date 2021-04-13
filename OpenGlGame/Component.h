#pragma once
#include <string>
#include <utility>

#include "Types.h"

namespace Game
{
	class Entity;
	
	class Component
	{
		std::string m_Name;
		Entity *m_Entity;
	public:
		Component(std::string name, Entity* entity = nullptr) : m_Name(std::move(name)), m_Entity(entity) {}
		virtual ~Component() {}

		const std::string& Name() const { return m_Name; }

		Entity* GetEntity() const { return m_Entity; }
	};
}