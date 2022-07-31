#include "pch.h"
#include "Engine/Scene/Entity.h"

namespace Game
{
	Entity::Entity(const entt::entity &handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene)
	{}
}