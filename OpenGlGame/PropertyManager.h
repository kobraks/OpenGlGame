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

		sol::state *m_State = nullptr;
	public:
		PropertyManager() = default;

		bool Contains(const PropertyIdType &id) const { return m_Properties.contains(id); }
		void Remove(const PropertyIdType &id);

		size_t Size() const { return m_Properties.size(); }

		void Clear() { m_Properties.clear(); }
		void Clone(const PropertyManager &manager);

		void Register(const std::string &name, sol::state &state);

		PropertyList::iterator begin() { return m_Properties.begin(); }
		PropertyList::iterator end() { return m_Properties.end(); }

		PropertyList::const_iterator begin() const { return m_Properties.begin(); }
		PropertyList::const_iterator end() const { return m_Properties.end(); }

	private:
		Pointer<BaseProperty> GetBaseProperty(const std::string &name) const;
	
	public:
		template <typename T>
		void Set(const PropertyIdType &id, const T &value)
		{
			if(auto property = Find<T>(id); property)
				property->SetValue(value);
			else
				Add(Property<T>(id, value));
		}

		template <typename Type>
		bool Add(const PropertyIdType &id, const Type &value)
		{
			return AddProperty<Type>(Property<Type>(id, value));
		}
		
		template <typename Type>
		bool Add(const PropertyIdType &id, const Type &value, Property<Type>::SetterFunction onSet)
		{
			return AddProperty<Type>(Property<Type>(id, value, onSet));
		}

		template <typename Type>
		bool Add(const PropertyIdType &id, Property<Type>::GetterFunction getter, Property<Type>::SetterFunction onSet)
		{
			return AddProperty<Type>(Property<Type>(id, onSet, getter));
		}

		template <typename Type>
		bool Add(const Property<Type> &property)
		{
			return AddProperty<Type>(property);
		}


		template <typename Type>
		bool Add(const std::pair<PropertyIdType, Type> &pair)
		{
			return AddProperty<Type>(std::make_from_tuple<Property<Type>>(pair));
			return false;
		}

		template <typename Type>
		const Property<Type>& GetProperty(const PropertyIdType &id) const
		{
			auto property = Find<Type>(id);

			ASSERT(property, "No property with id: \"{}\" exists", id);

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		template <typename Type>
		Property<Type>& GetProperty(const PropertyIdType &id)
		{
			auto property = Find<Type>(id);

			ASSERT(property, "No property with id: \"{}\" exists", id);

			if(!property)
				throw std::runtime_error(fmt::format("No property with id: \"{}\" exists", id));

			return *property;
		}

		template <typename Type>
		Type Get(const PropertyIdType &id)
		{
			return GetProperty<Type>(id).Value();
		}

		template <typename Type>
		Type Get(const PropertyIdType &id) const
		{
			return GetProperty<Type>(id).Value();
		}


	private:
		template <typename T>
		Pointer<Property<T>> Find(const PropertyIdType &id) const
		{
			if(const auto &it = m_Properties.find(id); it != m_Properties.end())
				return std::dynamic_pointer_cast<Property<T>>(it->second);

			return nullptr;
		}

		template <typename T>
		bool AddProperty(const Property<T> &property)
		{
			if(!Contains(property.Id()))
			{
				auto it = m_Properties.emplace(std::make_pair(property.Id(), property.Clone()));

				return true;
			}
			return false;
		}

		template <typename T>
		bool AddProperty(Property<T> &&property)
		{
			if(!Contains(property.Id()))
			{
				auto it = m_Properties.emplace(std::make_pair(property.Id(), MakePointer<Property<T>>(std::forward<Property<T>>(property))));

				return true;
			}

			return false;
		}
	};
}
