#pragma once
#include "Types.h"

#include <string>
#include <string_view>
#include <typeinfo>
#include <functional>
#include <optional>

#include <sol/state.hpp>
#include <sol/object.hpp>

#include "Assert.h"

#define GET_TYPE_NAME(Type) typeid(Type).name()

namespace Game
{
	using PropertyIDType = std::string;

	class PropertyManager;
	
	class BaseProperty
	{
	public:
		class Type
		{
			std::string m_Name;
		public:
			Type(std::string name) : m_Name(std::move(name)) {}
			std::string_view Name() const { return m_Name; }
		};

	private:
		const PropertyIDType m_Id;
		Type m_Type;

	public:
		BaseProperty(std::string typeName, PropertyIDType id);
		virtual ~BaseProperty() = default;

		const Type& Type() const { return m_Type; }
		const PropertyIDType& ID() const { return m_Id; }

		virtual Pointer<BaseProperty> Clone() const = 0;


	protected:
		virtual sol::object LuaGet(sol::state& state)
		{
			return sol::nil;
		}
		
		virtual void LuaSet(sol::object) {}

		friend class PropertyManager;
	};

	template <typename T>
	class Property: public BaseProperty
	{
	public:
		using ValueType = T;
		using SetterFunctionType = std::function<void(ValueType)>;
		using GetterFunctionType = std::function<ValueType()>;

	private:
		SetterFunctionType m_Setter = nullptr;
		GetterFunctionType m_Getter = nullptr;

		std::optional<ValueType> m_Value = std::nullopt;

	public:
		explicit Property(PropertyIDType id) : BaseProperty(GET_TYPE_NAME(T), std::move(id)) {}

		Property(PropertyIDType id, const ValueType &value) : Property(std::move(id))
		{
			m_Value = value;
		}

		Property(PropertyIDType id, SetterFunctionType onSet, GetterFunctionType onGet) : Property(std::move(id))
		{
			m_Getter = onGet;
			m_Setter = onSet;
		}

		Property(PropertyIDType id, const ValueType &value, SetterFunctionType onSet) : Property(std::move(id), value)
		{
			m_Setter = onSet;
		}

		[[nodiscard]] Pointer<BaseProperty> Clone() const override
		{
			auto pointer = MakePointer<Property<T>>(ID());

			Property<T> &property = *pointer;
			property.m_Getter     = m_Getter;
			property.m_Setter     = m_Setter;
			property.m_Value      = m_Value;

			return pointer;
		}

		ValueType Value() const
		{
			if(m_Getter)
				return m_Getter();

			if(m_Value)
				return m_Value.value();

			ASSERT(false, "Property value not set");
			throw std::runtime_error("Property value not set");
		}

		void Value(const ValueType &value)
		{
			if (m_Setter)
				return m_Setter(value);

			m_Value.emplace(value);
		}

		Property<ValueType>& operator=(const ValueType &value)
		{
			Value(value);
			return *this;
		}

		const ValueType& operator*() const { return Value(); }

	private:
		sol::object LuaGet(sol::state &state) override
		{
			return sol::make_object<ValueType>(state, Value());
		}

		void LuaSet(sol::object object) override
		{
			if (object == sol::nil || !object.is<ValueType>())
				return;

			Value(object.as<ValueType>());
		}
		
		friend class PropertyManager;
	};
}
