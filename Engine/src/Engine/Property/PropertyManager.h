#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Lua/LuaRegister.h"

#include <unordered_map>
#include <sol/object.hpp>

namespace Game
{
	class PropertyManager : public LuaRegister
	{
		using PropertyList = std::unordered_map<PropertyIDType, Pointer<BaseProperty>>;

		PropertyList m_Properties;
	public:
		PropertyManager() = default;

		bool Contains(const PropertyIDType &id) const { return m_Properties.contains(id); }
		void Remove(const PropertyIDType &id);

		size_t Size() const { return m_Properties.size(); }

		void Clear() { m_Properties.clear(); }
		void Clone(const PropertyManager &manager);

		PropertyList::iterator begin() { return m_Properties.begin(); }
		PropertyList::iterator end() { return m_Properties.end(); }

		PropertyList::const_iterator begin() const { return m_Properties.begin(); }
		PropertyList::const_iterator end() const { return m_Properties.end(); }

	protected:
		sol::object LuaGet(sol::this_state state, const PropertyIDType &id);
		void LuaSet(const PropertyIDType &id, sol::object value);
		bool LuaAdd(const PropertyIDType &id, sol::variadic_args args);

		sol::table LuaGetNames(sol::this_state state) const;
		sol::table LuaGetAll(sol::this_state state) const;

	private:
		Pointer<BaseProperty> GetBaseProperty(const std::string &name) const;
		void Register(sol::state &state) override;

	public:
		template <typename Type>
		void Set(const PropertyIDType &id, const Type &value)
		{
			if(auto property = Find<Type>(id); property)
				property->SetValue(value);
			else
				Add(Property<Type>(id, value));
		}

		template <typename Type>
		bool Add(const PropertyIDType &id, const Type &value)
		{
			return AddProperty<Type>(Property<Type>(id, value));
		}

		template <typename Type>
		bool Add(
			const PropertyIDType &id,
			typename Property<Type>::GetterFunctionType getter,
			typename Property<Type>::SetterFunctionType onSet
			)
		{
			return AddProperty<Type>(Property<Type>(id, onSet, getter));
		}

		template <typename Type>
		bool Add(const Property<Type> &property)
		{
			return AddProperty<Type>(property);
		}

		template <typename Type>
		bool Add(const std::pair<PropertyIDType, Type> &pair)
		{
			return AddProperty<Type>(std::make_from_tuple<Property<Type>>(pair));
		}

		template <typename Type>
		const Property<Type>& GetProperty(const PropertyIDType &id) const
		{
			auto property = Find<Type>(id);

			ASSERT(property, fmt::format("No property with id: \"{}\" exists", id));

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		template <typename Type>
		Property<Type>& GetProperty(const PropertyIDType &id)
		{
			auto property = Find<Type>(id);

			ASSERT(property, fmt::format("No property with id: \"{}\" exists", id));

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		template <typename Type>
		Type Get(const PropertyIDType &id)
		{
			return GetProperty<Type>(id).Value();
		}

		template <typename Type>
		Type Get(const PropertyIDType &id) const
		{
			return GetProperty<Type>(id).Value();
		}


	private:
		template <typename T>
		Pointer<Property<T>> Find(const PropertyIDType &id) const
		{
			if(const auto &it = m_Properties.find(id); it != m_Properties.end())
				return std::dynamic_pointer_cast<Property<T>>(it->second);

			return nullptr;
		}

		template <typename T>
		bool AddProperty(const Property<T> &property)
		{
			if(!Contains(property.ID()))
			{
				auto it = m_Properties.emplace(std::make_pair(property.ID(), property.Clone()));

				return true;
			}
			return false;
		}

		template <typename T>
		bool AddProperty(Property<T> &&property)
		{
			if(!Contains(property.ID()))
			{
				auto it = m_Properties.emplace(
				                               std::make_pair(
				                                              property.ID(),
				                                              MakePointer<Property<T>>(
					                                               std::forward<Property<T>>(property)
					                                              )
				                                             )
				                              );

				return true;
			}

			return false;
		}
	};
}
