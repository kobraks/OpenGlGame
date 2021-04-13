#include "pch.h"
#include "Scene.h"


#include "CameraComponent.h"
#include "Entity.h"
#include "ModelComponent.h"
#include "NativeScirptComponent.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "TagComponent.h"

namespace Game
{
	Scene::Scene() { }
	Scene::~Scene() {}

	Entity Scene::CreateEntity(const std::string &name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<TransformComponent>();
		auto &tag = entity.AddComponent<TagComponent>();
		tag.Tag   = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(const Entity &entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate()
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

			                                              nsc.Instance->OnUpdate();
		                                              }
		                                             );

		Camera *mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for(auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if(camera.IsPrimary())
				{
					mainCamera      = &camera.GetCamera();
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if(mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<ModelComponent>);
			for(auto entity : group)
			{
				auto [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
				model.Draw(transform.GetTransform());
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnUpdateRuntime(const Time &timeStep)
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
			if(!cameraComponent.HasFixedAspectRatio())
				cameraComponent.GetCamera().SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for(auto entity : view)
		{
			const auto &camera = view.get<CameraComponent>(entity);
			if(camera.IsPrimary())
				return Entity{entity, this};
		}

		return {};
	}
}
