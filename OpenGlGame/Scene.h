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

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string &name = {});
		void DestroyEntity(const Entity &entity);

		void OnUpdate();
		void OnUpdateRuntime(const Time &timeStep);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

	private:
		template <typename Component>
		void OnComponentAdded(Entity& entity, Component& component);
		
		friend class Entity;
		friend class SceneSerializer;
		
	};
}
