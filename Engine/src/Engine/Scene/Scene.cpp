#include "pch.h"
#include "Engine/Scene/Scene.h"

#include "Entity.h"
#include "Engine/Scene/Components.h"

namespace {
	template <typename... Component>
	void CopyComponent(entt::registry &dst, entt::registry &src, const std::unordered_map<Engine::UUID, entt::entity> &enttMap) {
		([&](){
			auto view = src.view<Component>();
			for (auto srcEntity : view) {
				entt::entity dstEntity = enttMap.at(src.get<Engine::IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template <typename... Component>
	void CopyComponent(Engine::ComponentGroup<Component...>, entt::registry &dst, entt::registry &src, const std::unordered_map<Engine::UUID, entt::entity> &enttMap) {
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template <typename ...Component>
	void CopyComponentIfExists(Engine::Entity dst, Engine::Entity src) {
		([&]() {
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template <typename ...Component>
	void CopyComponentIfExists(Engine::ComponentGroup<Component...>, Engine::Entity dst, Engine::Entity src) {
		CopyComponentIfExists<Component...>(dst, src);
	}
}

namespace Engine {

	Scene::Scene(const std::string &title) : m_Title(title) {}

	Scene::~Scene() {}

	Ref<Scene> Scene::Copy(Ref<Scene> other) {
		Ref<Scene> newScene = MakeRef<Scene>();

		newScene->m_ViewportHeight = other->m_ViewportHeight;
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_Title = other->m_Title;

		auto &srcSceneReg = other->m_Registry;
		auto &dstSceneReg = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcSceneReg.view<IDComponent>();
		for (auto e : idView) {
			UUID uuid = srcSceneReg.get<IDComponent>(e).ID;
			const auto &name = srcSceneReg.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntity(uuid, name);

			enttMap[uuid] = static_cast<entt::entity>(newEntity);
		}

		CopyComponent(AllComponents{}, dstSceneReg, srcSceneReg, enttMap);

		return newScene;
	}
	Entity Scene::CreateEntity(const std::string &name) {
		return CreateEntity(UUID(), name);
	}

	Entity Scene::CreateEntity(UUID id, const std::string &name) {
		Entity entity = CreateEmpty();
		entity.AddComponent<IDComponent>(id);

		auto& tag = entity.AddComponent<TagComponent>();
		entity.AddComponent<TransformComponent>();

		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	Entity Scene::GetPrimaryCameraEntity() {
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity: view) {
			const auto &camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{entity, this};
		}

		return {};
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height) {
		m_ViewportHeight = height;
		m_ViewportWidth = width;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto &camera = view.get<CameraComponent>(entity);
			if (!camera.FixedAspectRatio)
				camera.Camera.SetViewportSize(width, height);
		}
	}

	void Scene::DuplicateEntity(Entity entity) {
		Entity newEntity = CreateEntity(entity.GetName());
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
	}

	Entity Scene::CreateEmpty() {
		return {m_Registry.create(), this};
	}

	Entity Scene::CreateEmpty(UUID id) {
		Entity entity{m_Registry.create(), this};
		entity.AddComponent<IDComponent>(id);

		return entity;
	}

	template <typename Component>
	void Scene::OnComponentAdded(Entity &entity, Component &component) {
		ENGINE_ASSERT(false, "Unknown component");
	}

	template <>
	void Scene::OnComponentAdded(Entity &entity, IDComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, TagComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, TransformComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, CameraComponent &component) {
		if (component.FixedAspectRatio)
			return;

		if (m_ViewportHeight > 0 && m_ViewportWidth > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
