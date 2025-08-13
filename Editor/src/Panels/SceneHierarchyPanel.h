#pragma once

#include <Engine/Core/Base.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/Entity.h>

#include "SceneContextView.h"

namespace Editor
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const SceneContextView &context);

		void SetContext(const SceneContextView &context);

		void OnImGuiRender();

		Engine::Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Engine::Entity entity);


	private:
		void DrawEntityNode(Engine::Entity entity);
		void DrawComponents(Engine::Entity entity);

		SceneContextView m_Context;
		Engine::Entity m_SelectionContext;
	private:
		template <typename T>
		void DisplayAddComponentEntry(const std::string &entryName);
	};
}