#pragma once

#include <Engine/Core/Base.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/Entity.h>

namespace Game
{
	class SceneHierarchyPanel
	{
		Engine::Ref<Engine::Scene> m_Context;
		Engine::Entity m_SelectionContext;

	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context);

		void SetContext(const Engine::Ref<Engine::Scene> &context);

		void OnImGuiRender();

		Engine::Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Engine::Entity entity);


	private:
		void DrawEntityNode(Engine::Entity entity);
		void DrawComponents(Engine::Entity entity);

	private:
		template <typename T>
		void DisplayAddComponentEntry(const std::string &entryName);
	};
}