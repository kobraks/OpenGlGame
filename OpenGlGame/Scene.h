#pragma once
#include <string>
#include <entt/entt.hpp>

#include "Time.h"

namespace Game
{
	class Entity;

	class Scene
	{
		entt::registry m_Registry;

		uint32_t m_ViewportWidth  = 0;
		uint32_t m_ViewportHeight = 0;

		std::string m_Title = {};
	public:
		Scene(const std::string &title = "Untitled");
		~Scene();

		Entity CreateEntity(const std::string &tag = {});
		Entity CreateEntity(uint32_t hint, const std::string &tag = {});
		void DestroyEntity(const Entity &entity);

		void OnUpdate();
		void OnConstUpdate(const Time &timeStep);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		std::string Title() const { return m_Title; }
		void SetTitle(const std::string &title) { m_Title = title; }

	private:
		template <typename Component>
		void OnComponentAdded(Entity &entity, Component &component);
	
	private:
		Entity CreateEmpty();
		Entity CreateEmpty(uint32_t hint);

		friend class Entity;
		friend class SceneSerializer;
	};
}
