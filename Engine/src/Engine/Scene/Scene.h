#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Time.h"

#include <entt.hpp>

namespace Editor {
	class SceneHierarchyPanel;
}

namespace Engine {
	class Entity;
	class Camera;
	class SceneSerializer;

	class Scene {
	public:
		explicit Scene(const std::string &title = "Untitled");
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string &name = "");
		Entity CreateEntity(UUID id, const std::string &name = "");

		void DestroyEntity(Entity entity);
		Entity GetPrimaryCameraEntity();

		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		std::string_view Title() const { return m_Title; }
		void SetTitle(const std::string &title) { m_Title = title; }

	private:
		Entity CreateEmpty();
		Entity CreateEmpty(UUID id);

		template <typename Component>
		void OnComponentAdded(Entity &entity, Component &component);

	private:
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		std::string m_Title = {};

		friend class Entity;
		friend class SceneSerializer;
		friend class Editor::SceneHierarchyPanel;
	};
}
