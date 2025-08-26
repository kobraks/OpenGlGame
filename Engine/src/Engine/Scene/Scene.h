#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"
#include "Engine/Core/Time.h"

#include <entt.hpp>

namespace Editor {
	class SceneHierarchyPanel;
}

namespace Engine {
	class EditorCamera;
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

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime();
		void OnConstUpdateRuntime(const Time &ts);

		void OnUpdateSimulation(EditorCamera& camera);
		void OnConstUpdateSimulation(const Time& ts, EditorCamera& camera);

		void OnUpdateEditor(EditorCamera& camera);
		void OnConstUpdateEditor(const Time& ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);
		Entity FindEntityByName(std::string_view name);
		Entity GetEntityByUUID(UUID uuid);

		Entity GetPrimaryCameraEntity();

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused;  }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void Step(uint32_t frames = 1);

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
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		uint32_t m_StepFrames = 0;

		std::string m_Title = {};

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class Editor::SceneHierarchyPanel;
	};
}
