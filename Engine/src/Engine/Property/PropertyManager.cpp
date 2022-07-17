#include "pch.h"
#include "PropertyManager.h"

#include "Engine/Lua/LuaUtils.h"

namespace Game
{
	static bool s_Registred = false;

	static bool AddLuaProperty(PropertyManager &manager, const PropertyIDType &id, sol::object value)
	{
		return manager.Add<sol::object>(id, value);
	}

	static bool AddLuaProperty(
		PropertyManager &manager,
		const PropertyIDType &id,
		sol::object setterObject,
		sol::object getterObject
		)
	{
		ASSERT(
		       setterObject.is<sol::function>() && getterObject.is<sol::function>(),
		       "You must provide get and set functions"
		      );

		if(!(setterObject.is<sol::function>() && getterObject.is<sol::function>()))
			throw std::runtime_error(fmt::format("Get and Set must be functions"));

		using Property = Property<sol::object>;
		auto setter = setterObject.as<Property::SetterFunctionType>();
		auto getter = getterObject.as<Property::GetterFunctionType>();

		return manager.Add<sol::object>(id, getter, setter);
	}

	void PropertyManager::Remove(const PropertyIDType &id)
	{
		if(auto it = m_Properties.find(id); it != m_Properties.end())
			m_Properties.erase(it);
	}

	void PropertyManager::Clone(const PropertyManager &manager)
	{
		for(const auto &[id, property] : manager)
			m_Properties[id] = property->Clone();
	}

	sol::object PropertyManager::LuaGet(sol::this_state state, const PropertyIDType &id)
	{
		if (auto prop = GetBaseProperty(id); prop)
			return prop->LuaGet(state);
		return sol::nil;
	}

	void PropertyManager::LuaSet(const PropertyIDType &id, sol::object value)
	{
		if (auto prop = GetBaseProperty(id); prop)
			return prop->LuaSet(value);

		AddLuaProperty(*this, id, value);
	}

	bool PropertyManager::LuaAdd(const PropertyIDType &id, sol::variadic_args args)
	{
		if (args.size() > 2)
		{
			ASSERT(false, "Too many Arguments");
			return false;
		}

		if (args.size() == 0)
			return AddLuaProperty(*this, id, sol::nil);
		if (args.size() == 1)
			return AddLuaProperty(*this, id, args[0].as<sol::object>());
		if (args.size() == 2)
			return AddLuaProperty(*this, id, args[0].as<sol::object>(), args[1].as<sol::object>());

		return false;
	}

	sol::table PropertyManager::LuaGetNames(sol::this_state state) const
	{
		//TODO some sort of cashing
		sol::table table(state, sol::new_table(m_Properties.size()));

		int i = 1;
		for (auto it : m_Properties)
			table[i++] = it.first;

		return table;

	}

	sol::table PropertyManager::LuaGetAll(sol::this_state state) const
	{
		//TODO some sort of cashing

		sol::table table(state, sol::new_table(m_Properties.size()));

		int i = 1;
		for (auto it : m_Properties)
		{
			auto local = table[i++] = sol::table(state, sol::new_table(0, 2));

			local["Name"] = it.first;
			local["Value"] = it.second->LuaGet(state);
		}

		return table;
	}

	Pointer<BaseProperty> PropertyManager::GetBaseProperty(const std::string &name) const
	{
		if(const auto iter = m_Properties.find(name); iter != m_Properties.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void PropertyManager::Register(sol::state &state)
	{
		auto ut = state.new_usertype<PropertyManager>("PropertyManager");

		ut["Get"]      = &PropertyManager::LuaGet;
		ut["GetNames"] = &PropertyManager::LuaGetNames;
		ut["GetAll"]   = &PropertyManager::LuaGetAll;
		ut["Set"]      = &PropertyManager::LuaSet;
		ut["Add"]      = &PropertyManager::LuaAdd;

		ut["Clear"] = &PropertyManager::Clear;
		ut["Remove"] = &PropertyManager::Remove;

		ut["Contains"] = &PropertyManager::Contains;
		ut["Size"] = &PropertyManager::Size;
		ut["Clone"] = &PropertyManager::Clone;
		
	}
}
