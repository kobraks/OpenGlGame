#include "pch.h"
#include "PropertyManager.h"

#include "LuaUtils.h"

namespace Game
{
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

	void PropertyManager::Register(const std::string &name, sol::state &state)
	{
		m_State = &state;

		auto metaTable = state.create_table();

		metaTable["Get"] = [this](PropertyIDType id) ->sol::object
		{
			if(auto prop = this->GetBaseProperty(id); prop)
				return prop->LuaGet(*this->m_State);
			return sol::nil;
		};

		metaTable["GetProperty"] = [this](PropertyIDType id) -> sol::object
		{
			if (auto prop = this->GetBaseProperty(id); prop)
			{
				auto table = m_State->create_table(0, 2);
				table["Name"] = prop->ID();
				table["Value"] = prop->LuaGet(*m_State);

				return table;
			}

			return sol::nil;
		};

		metaTable["Set"] = [this](PropertyIDType id, sol::object value)
		{
			if(auto prop = this->GetBaseProperty(id); prop)
				prop->LuaSet(value);
			else
				AddLuaProperty(*this, id, value);
		};

		metaTable["Add"] = [this](PropertyIDType id, sol::variadic_args args)
		{
			if(args.size() > 2)
			{
				assert(false);
				return false;
			}

			if(args.size() == 1)
				return AddLuaProperty(*this, id, args[0].as<sol::object>());
			if(args.size() == 2)
				return AddLuaProperty(*this, id, args[0].as<sol::object>(), args[1].as<sol::object>());
		};

		metaTable["Remove"] = [this](PropertyIDType id)
		{
			return this->Remove(id);
		};

		metaTable["Clear"] = [this](PropertyIDType id)
		{
			return this->Clear();
		};

		metaTable["Contains"] = [this](PropertyIDType id)
		{
			return this->Contains(id);
		};

		metaTable["Size"] = [this](PropertyIDType id)
		{
			return this->Size();
		};

		metaTable["GetNames"] = [this]()
		{
			auto table = this->m_State->create_table(m_Properties.size(), 0);

			int i = 1;
			for (auto it : m_Properties)
				table[i++] = it.first.c_str();

			return table;
		};

		metaTable["GetAll"] = [this]()
		{
			auto table = m_State->create_table(m_Properties.size(), 0);

			int i = 1;
			for (auto it : m_Properties)
			{
				auto local = table[i++] = m_State->create_table(0, 2);

				local["Name"] = it.first;
				local["Value"] = it.second->LuaGet(*m_State);
			}

			return table;
		};

		auto table = state.create_table(name);

		SetAsReadOnlyTable(table, metaTable);
	}

	Pointer<BaseProperty> PropertyManager::GetBaseProperty(const std::string &name) const
	{
		if(const auto iter = m_Properties.find(name); iter != m_Properties.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}
