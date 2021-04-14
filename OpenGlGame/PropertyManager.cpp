#include "pch.h"
#include "PropertyManager.h"

namespace Game
{
	static uint32_t s_RegisterIndex = 0;
	
	void PropertyManager::Remove(const PropertyIdType &id)
	{
		if(auto it = m_Properties.find(id); it != m_Properties.end())
			m_Properties.erase(it);
	}

	void PropertyManager::Clone(const PropertyManager &manager)
	{
		for(const auto &[id, property] : manager)
			m_Properties[id] = property->Clone();
	}

	void PropertyManager::Register(const std::string &name, sol::state &state)
	{
		const auto typeName = fmt::format("PropertyManager{}", s_RegisterIndex++);

		m_State = &state;

		state.set(name, *this);

		state.new_usertype<PropertyManager>(typeName);

		state[typeName]["Get"] = [this](std::string name) -> sol::object {if (auto prop = this->GetBaseProperty(name); prop) return prop->LuaGet(*this->m_State); return sol::nil;};
		state[typeName]["Set"] = [this](std::string name, sol::object value) -> void {if (auto prop = this->GetBaseProperty(name); prop) prop->LuaSet(value); };
	}

	Pointer<BaseProperty> PropertyManager::GetBaseProperty(const std::string &name) const
	{
		if (const auto iter = m_Properties.find(name); iter != m_Properties.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}
