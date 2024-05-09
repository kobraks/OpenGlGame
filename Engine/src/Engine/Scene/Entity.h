#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Scene.h"

#include <entt.hpp>

namespace Engine {
	class Entity {
	public:
		Entity() = default;
		Entity(const Entity&) = default;
		Entity(const entt::entity &handle, Scene *scene);

		UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() const { return GetComponent<TagComponent>().Tag; }

		constexpr bool operator==(const Entity &entity) const noexcept {
			return m_Scene == entity.m_Scene && m_EntityHandle == entity.m_EntityHandle;
		}

		constexpr bool operator!=(const Entity &entity) const noexcept {
			return !(*this == entity);
		}

		constexpr operator bool() const noexcept { return m_EntityHandle != entt::null; }
		constexpr operator entt::entity() const noexcept { return m_EntityHandle; }
		constexpr operator uint32_t() const noexcept { return static_cast<uint32_t>(m_EntityHandle); }
	public:
		template <typename Component, typename ...Args>
		Component& AddComponent(Args&&... args);
		template <typename Component, typename ...Args>
		Component& AddOrReplaceComponent(Args&&... args);
		template <typename Component, typename ...Args>
		Component& ReplaceComponent(Args&&... args);
		template <typename Component>
		Component& GetComponent() const;
		template <typename Component>
		void RemoveComponent();
		template <typename Component>
		[[nodiscard]] bool HasComponent() const;
		template <typename ...Components>
		[[nodiscard]] bool HasAny() const;
		template <typename ...Components>
		[[nodiscard]] bool HasAll() const;

	private:
		entt::entity m_EntityHandle{entt::null};
		Scene *m_Scene = nullptr;
	};

	template <typename Component, typename ... Args>
	Component & Entity::AddComponent(Args &&...args) {
		ENGINE_ASSERT(!HasComponent<Component>());
		Component& component = m_Scene->m_Registry.emplace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		m_Scene->OnComponentAdded<Component>(*this, component);

		return component;
	}

	template <typename Component, typename ... Args>
	Component & Entity::AddOrReplaceComponent(Args &&...args) {
		Component& component = m_Scene->m_Registry.emplace_or_replace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		m_Scene->OnComponentAdded<Component>(*this, component);

		return component;
	}

	template <typename Component, typename ... Args>
	Component & Entity::ReplaceComponent(Args &&...args) {
		ENGINE_ASSERT(HasComponent<Component>())
		Component& component = m_Scene->m_Registry.replace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		m_Scene->OnComponentAdded<Component>(*this, component);

		return component;
	}

	template <typename Component>
	Component & Entity::GetComponent() const {
		ENGINE_ASSERT(HasComponent<Component>());
		return m_Scene->m_Registry.get<Component>(m_EntityHandle);
	}

	template <typename Component>
	void Entity::RemoveComponent() {
		ENGINE_ASSERT(HasComponent<Component>());
		m_Scene->m_Registry.remove<Component>(m_EntityHandle);
	}

	template <typename Component>
	bool Entity::HasComponent() const {
		return m_Scene->m_Registry.all_of<Component>(m_EntityHandle);
	}

	template <typename ... Components>
	bool Entity::HasAny() const {
		return m_Scene->m_Registry.any_of<Components...>(m_EntityHandle);
	}

	template <typename ... Components>
	bool Entity::HasAll() const {
		return m_Scene->m_Registry.all_of<Components...>(m_EntityHandle);
	}
}
