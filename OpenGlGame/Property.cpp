#include "pch.h"
#include "Property.h"

namespace Game
{
	BaseProperty::BaseProperty(std::string typeName, PropertyIDType id) : m_Id(std::move(id)), m_Type({std::move(typeName)})
	{
		
	}
}
