#pragma once

#include <Engine/Core/Base.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/Entity.h>

namespace Game
{
	class SceneHierarchyPanel
	{
		Ref<Scene> m_Context;
		Entity m_SelectionContext;

	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene> &context);

		void SetContext(const Ref<Scene> &context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);


	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		template <typename T>
		void DisplayAddComponentEntry(const std::string &entryName);
	};
}