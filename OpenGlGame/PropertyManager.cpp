#include "pch.h"
#include "PropertyManager.h"

namespace Game
{
	bool PropertyManager::Add(Pointer<BaseProperty> property)
	{
		if (!Contains(property->Id()))
		{
			// auto it = m_Properties.emplace(property->Clone());

			// if (m_Object)
				// it.first->second->Register(m_Object);

			return true;
		}

		return false;
	}
	
	void PropertyManager::Remove(const PropertyIdType &id)
	{
		if (auto it = m_Properties.find(id); it != m_Properties.end())
		{
			if (m_Object)
				m_Object[id] = sol::nil;

			m_Properties.erase(it);
		}
	}
	
	void PropertyManager::Clone(const PropertyManager &manager)
	{
		for (const auto& [id, property] : manager)
		{
			m_Properties[id] = property->Clone();

			if (m_Object)
				property->Register(m_Object);
		}
	}

	void PropertyManager::Register(const std::string &name, sol::state &state)
	{
		
	}
}
