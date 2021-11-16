#pragma once
#include <string>
#include <entt/entt.hpp>

#include "Time.h"

namespace Game
{
	class Entity;
	class Camera;
	class UUID;
	
	class Scene
	{
		entt::registry m_Registry;

		uint32_t m_ViewportWidth  = 0;
		uint32_t m_ViewportHeight = 0;

		std::string m_Title = {};

		//Updated on every OnUpdate call
		Camera *m_MainCamera = nullptr;
		glm::mat4 m_CameraTransform = glm::mat4(1.f);

	public:
		Scene(const std::string &title = "Untitled");
		~Scene();

		Entity CreateEntity(const std::string &name = {});
		Entity CreateEntity(UUID uuid, const std::string &name = {});
		void DestroyEntity(const Entity &entity);

		void OnUpdate();
		void OnDraw();
		void OnConstUpdate(const Time &timeStep);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		std::string Title() const { return m_Title; }
		void SetTitle(const std::string &title) { m_Title = title; }

	private:
		template <typename Component>
		void OnComponentAdded(Entity &entity, Component &component);

		Entity CreateEmpty();
		Entity CreateEmpty(UUID uuid);
	private:
		friend class Entity;
		friend class SceneSerializer;
	};
}
