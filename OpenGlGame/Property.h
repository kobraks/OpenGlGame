#pragma once
#include "Types.h"
#include <string>
#include <string_view>
#include <typeinfo>
#include <functional>
#include <optional>

#include "Assert.h"

#define GET_TYPE_NAME(Type) typeid(Type).name()

namespace Game
{
	using PropertyIdType = std::string;

	class BaseProperty
	{
	public:
		class Type
		{
			std::string m_Name;
		public:
			Type(const std::string &name) : m_Name(name) {}
			Type(std::string &&name) : m_Name(std::move(name)) {}
			std::string_view Name() const { return m_Name; }
		};

	private:
		const PropertyIdType m_Id;
		Type m_Type;

	public:
		BaseProperty(const std::string &typeName, const PropertyIdType &id);
		BaseProperty(const std::string &typeName, PropertyIdType &&id);
		BaseProperty(std::string &&typeName, PropertyIdType &&id);
		BaseProperty(std::string &typeName, const PropertyIdType &id);

		const Type& Type() const { return m_Type; }
		const PropertyIdType& Id() const { return m_Id; }

		virtual Pointer<BaseProperty> Clone() const = 0;
		virtual void Register(sol::object& object) const = 0;
	};

	template <typename T>
	class Property: public BaseProperty
	{
	public:
		using ValueType = T;
		using SetterFunction = std::function<void(const ValueType &)>;
		// using GetterFunction = std::function<ValueType()>;

	private:
		SetterFunction m_Setter = nullptr;
		// GetterFunction m_Getter = nullptr;

		std::optional<ValueType> m_Value = std::nullopt;

	public:
		explicit Property(const PropertyIdType &id) : BaseProperty(GET_TYPE_NAME(T), id) {}
		explicit Property(PropertyIdType &&id) : BaseProperty(GET_TYPE_NAME(T), std::move(id)) {}

		Property(const PropertyIdType &id, const ValueType &value) : Property(id)
		{
			m_Value = value;
		}

		[[nodiscard]] Pointer<BaseProperty> Clone() const override
		{
			auto pointer = MakePointer<Property<T>>(Id());

			Property<T> &property = *pointer;
			// property.m_Getter     = m_Getter;
			property.m_Setter     = m_Setter;
			property.m_Value      = m_Value;

			return pointer;
		}

		void Register(sol::object& object) const override
		{
			// object[Id()] = sol::property(&Property<T>::Value, &Property<T>::SetValue);
		}

		const ValueType& Value() const
		{
			if(m_Value)
				return m_Value.value();

			ASSERT(false, "Property value not set");
			throw std::runtime_error("Property value not set");
		}

		ValueType& Value()
		{
			if(m_Value)
				return m_Value.value();
			ASSERT(false, "Property value not set");
			throw std::runtime_error("Property value not set");
		}

		void SetValue(const ValueType& value)
		{
			m_Value = value;
		}

		Property<ValueType>& operator= (const ValueType& value)
		{
			SetValue(value);
			return *this;
		}

		const ValueType& operator*() const { return Value(); }
		ValueType& operator*() { return Value(); }
	};
}
