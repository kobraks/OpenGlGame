#include "pch.h"
#include "Property.h"

namespace Game
{
	BaseProperty::BaseProperty(const std::string &typeName, const PropertyIdType &id) : m_Id(id),
	                                                                                    m_Type(typeName) {}

	BaseProperty::BaseProperty(const std::string &typeName, PropertyIdType &&id) : m_Id(std::move(id)),
	                                                                               m_Type(typeName) {}

	BaseProperty::BaseProperty(std::string &&typeName, PropertyIdType &&id) : m_Id(std::move(id)),
	                                                                          m_Type(std::move(typeName)) {}

	BaseProperty::BaseProperty(std::string &typeName, const PropertyIdType &id) : m_Id(id),
	                                                                              m_Type(std::move(typeName)) {}
}
