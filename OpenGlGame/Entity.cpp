#include "pch.h"
#include "Entity.h"

#include "EntityLuaHandle.h"
#include "Component.h"

namespace Game
{
	Entity::Entity(const size_t &id, const std::string &name) : m_Id(id), m_Name(name), m_LuaHandle(MakePointer<EntityLuaHandle>(this)){}

	void Entity::SetLuaHandle(Pointer<EntityLuaHandle> handle)
	{
		if (handle->GetEntity() != this)
			handle->SetEntity(this);

		m_LuaHandle = handle;
	}

	void Entity::AddComponent(std::type_index index, Pointer<Component> component)
	{
		m_Components[index] = component;
		component->SetEntity(this);
	}

	Pointer<Entity> Entity::Clone() const
	{
		Pointer<Entity> entity = MakePointer<Entity>(m_Id, m_Name);

		for (const auto &i : m_Components)
			entity->AddComponent(i.first, i.second->Clone());

		return entity;
	}
}
