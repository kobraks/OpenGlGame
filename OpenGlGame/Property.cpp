#include "pch.h"
#include "Property.h"

namespace Game
{
	BaseProperty::BaseProperty(std::string typeName, PropertyIdType id) : m_Type({std::move(typeName)}), m_Id(std::move(id))
	{
		
	}
}
