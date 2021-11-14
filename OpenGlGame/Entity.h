#pragma once
#include <entt/entt.hpp>

#include "Types.h"
#include "PropertyManager.h"

#include "Scene.h"
#include "Component.h"

namespace Game
{
	class Component;
	class EntityLuaHandle;
	class Scene;
	
	class Entity
	{
	private:
		entt::entity m_EntityHandle {entt::null};
		Scene* m_Scene = nullptr;
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(const entt::entity& handle, Scene* scene);

		UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() const { return GetComponent<TagComponent>().Tag; }
		
		constexpr bool operator==(const Entity& entity) const noexcept
		{
			return m_Scene == entity.m_Scene && m_EntityHandle == entity.m_EntityHandle;
		}

		constexpr bool operator!=(const Entity& entity) const noexcept
		{
			return !(*this == entity);
		}

		constexpr operator bool() const noexcept { return m_EntityHandle != entt::null; }
		constexpr operator entt::entity() const noexcept { return m_EntityHandle; }
		constexpr operator uint32_t() const noexcept { return static_cast<uint32_t>(m_EntityHandle);}
	
	public:
		template <typename Component, typename ...Args>
		Component& AddComponent(Args&& ...args)
		{
			ASSERT(!HasComponent<Component>(), "Entity already has component");
			Component& component = m_Scene->m_Registry.emplace<Component>(m_EntityHandle, this, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<Component>(*this, component);
			return component;
		}

		template <typename Component, typename ...Args>
		Component& AddOrReplaceComponent(Args&& ...args)
		{
			Component& component = m_Scene->m_Registry.emplace_or_replace<Component>(m_EntityHandle, this, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<Component>(*this, component);
			return component;
		}
		
		template <typename Component>
		Component& GetComponent() const
		{
			ASSERT(HasComponent<Component>(), "Entity does not have component");
			return m_Scene->m_Registry.get<Component>(m_EntityHandle);
		}

		template <typename Component>
		void RemoveComponent()
		{
			ASSERT(HasComponent<Component>(), "Entity does not have component");
			m_Scene->m_Registry.remove<Component>(m_EntityHandle);
		}

		template <typename Component>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.has<Component>(m_EntityHandle);
		}
	};

}
