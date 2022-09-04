#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Time.h"

#include <string>
#include <entt.hpp>

namespace Game
{
	class Entity;
	class Camera;
	class UUID;

	class Scene
	{
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		std::string m_Title = {};

	public:
		explicit Scene(const std::string &title = "Untitled");
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string &name = std::string());
		Entity CreateEntity(UUID uuid, const std::string &name = std::string());

		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();

		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		std::string Title() const { return m_Title; }
		void SetTitle(const std::string &title)  { m_Title = title; }
	private:
		Entity CreateEmpty();
		Entity CreateEmpty(UUID uuid);

	public:
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template <typename Component>
		void OnComponentAdded(Entity &entity, Component &component);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}