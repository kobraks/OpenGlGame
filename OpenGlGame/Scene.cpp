#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Renderer.h"
#include "Component.h"

namespace Game
{
	Scene::Scene(const std::string &title) : m_Title(title) { }
	Scene::~Scene() {}

	Entity Scene::CreateEntity(const std::string &name)
	{
		return CreateEntity(UUID(), name);

		Entity entity = CreateEntity();

		entity.AddComponent<TransformComponent>();
		auto &tagc = entity.AddComponent<TagComponent>();
		tagc.Tag   = name.empty() ? "Entity" : name;

		return entity;
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string &name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		entity.AddComponent<TransformComponent>();

		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(const Entity &entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate()
	{
		m_MainCamera      = nullptr;
		m_CameraTransform = glm::mat4(1.f);

		m_Registry.view<NativeScriptComponent>().each(
		                                              [=](auto entity, auto &nsc)
		                                              {
			                                              if(!nsc.Instance)
			                                              {
				                                              nsc.Instance           = nsc.InstantiateScript();
				                                              nsc.Instance->m_Entity = Entity{entity, this};
				                                              nsc.Instance->OnCreate();
			                                              }

			                                              nsc.Instance->OnUpdate();
		                                              }
		                                             );

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for(auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if(camera.Primary)
				{
					m_MainCamera      = &camera.Camera;
					m_CameraTransform = transform.GetTransform();
					break;
				}
			}
		}
	}

	void Scene::OnDraw()
	{
		if(m_MainCamera)
		{
			Renderer::BeginScene(*m_MainCamera, m_CameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<ModelComponent>);
			for(auto entity : group)
			{
				auto [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
				if(model.Drawable)
					model.Draw(transform.GetTransform());
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnConstUpdate(const Time &timeStep)
	{
		m_Registry.view<NativeScriptComponent>().each(
		                                              [=](auto entity, auto &nsc)
		                                              {
			                                              if(!nsc.Instance)
			                                              {
				                                              nsc.Instance           = nsc.InstantiateScript();
				                                              nsc.Instance->m_Entity = Entity{entity, this};
				                                              nsc.Instance->OnCreate();
			                                              }

			                                              nsc.Instance->OnConstUpdate(timeStep);
		                                              }
		                                             );
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportHeight = height;
		m_ViewportWidth  = width;

		auto view = m_Registry.view<CameraComponent>();
		for(auto entity : view)
		{
			auto &cameraComponent = view.get<CameraComponent>(entity);
			if(!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for(auto entity : view)
		{
			const auto &camera = view.get<CameraComponent>(entity);
			if(camera.Primary)
				return Entity{entity, this};
		}

		return {};
	}

	template <typename Component>
	void Scene::OnComponentAdded(Entity &entity, Component &component)
	{
		static_assert(false);
	}
	
	template <>
	void Scene::OnComponentAdded(Entity &entity, IDComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, TagComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, TransformComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, LuaScriptComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, ModelComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, NativeScriptComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, LightComponent &component) {}

	template <>
	void Scene::OnComponentAdded(Entity &entity, CameraComponent &component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}
