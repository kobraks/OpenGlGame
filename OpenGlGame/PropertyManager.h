#pragma once
#include <unordered_map>
#include <sol/object.hpp>

#include "Types.h"
#include "Property.h"

namespace Game
{
	class PropertyManager
	{
		using PropertyList = std::unordered_map<PropertyIdType, Pointer<BaseProperty>>;

		PropertyList m_Properties;
		sol::table m_Object;
	public:
		bool Contains(const PropertyIdType &id) const { return m_Properties.contains(id); }

		bool Add(Pointer<BaseProperty> property);
		void Remove(const PropertyIdType &id);

		size_t Size() const { return m_Properties.size(); }

		void Clear() { m_Properties.clear(); }
		void Clone(const PropertyManager &manager);

		template <typename Type>
		void Set(const PropertyIdType &id, const Type &value)
		{
			if(auto property = Find<Type>(id); property)
				property->SetValue(value);
			else
				Add(Property<Type>(id, value));
		}

		template <typename Type>
		void Set(const PropertyIdType &id, Type &&value)
		{
			if(auto property = Find<Type>(id); property)
				property->SetValue(std::forward<Type>(value));
			else
				Add(Property<Type>(id, std::forward<Type>(value)));
		}

		template <typename Type>
		bool Add(const PropertyIdType &id, const Type &value)
		{
			return AddProperty<Type>(Property<Type>(id, value));
		}

		template <typename Type>
		bool Add(const Property<Type> &property)
		{
			return AddProperty<Type>(property);
		}

		template <typename Type>
		bool Add(Property<Type> &&property)
		{
			return AddProperty<Type>(std::forward<Property<Type>>(property));
		}

		template <typename Type>
		bool Add(const std::pair<PropertyIdType, Type> &pair)
		{
			return AddProperty<Type>(std::make_from_tuple<Property<Type>>(pair));
			return false;
		}

		template <typename Type>
		const Property<Type>& Get(const PropertyIdType &id) const
		{
			auto property = Find<Type>(id);

			ASSERT(property, "No property with id: \"{}\" exists", id);

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		template <typename Type>
		Property<Type>& Get(const PropertyIdType &id)
		{
			auto property = Find<Type>(id);

			ASSERT(property, "No property with id: \"{}\" exists", id);

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		void Register(const std::string& name, sol::state& state);
		
		PropertyList::iterator begin() { return m_Properties.begin(); }
		PropertyList::iterator end() { return m_Properties.end(); }

		PropertyList::const_iterator begin() const { return m_Properties.begin(); }
		PropertyList::const_iterator end() const { return m_Properties.end(); }

	private:
		template <typename Type>
		Pointer<Property<Type>> Find(const PropertyIdType &id) const
		{
			if(const auto &it = m_Properties.find(id); it != m_Properties.end())
				return std::dynamic_pointer_cast<Property<Type>>(it->second);

			return nullptr;
		}

		template <typename Type>
		bool AddProperty(const Property<Type> &property)
		{
			if(!Contains(property.Id()))
			{
				auto it = m_Properties.emplace(std::make_pair(property.Id(), property.Clone()));;

				if(m_Object)
					it.first->second->Register(m_Object);

				return true;
			}

			return false;
		}

		template <typename Type>
		bool AddProperty(Property<Type> &&property)
		{
			if(!Contains(property.Id()))
			{
				auto it = m_Properties.emplace(std::make_pair(property.Id(), MakePointer<Property<Type>>(std::forward<Property<Type>>(property))));

				
				if(m_Object)
					it.first->second->Register(m_Object);

				return true;
			}

			return false;
		}
	};
}
