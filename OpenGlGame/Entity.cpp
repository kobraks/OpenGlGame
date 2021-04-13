#include "pch.h"
#include "Entity.h"

#include "EntityLuaHandle.h"
#include "Component.h"
#include "TransformComponent.h"

namespace Game
{
	Entity::Entity(const entt::entity &handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene)
 	{
	}
}
