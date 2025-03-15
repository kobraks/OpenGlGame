#pragma once

#include "Engine.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Editor
{
	class EditorLayer : public Engine::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate() override;
		void OnConstUpdate(const Engine::Time& timeStep) override;

		void OnImGuiRender() override;
		void OnEvent(Engine::Event &e) override;

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent &e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent &e);

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
		void SaveProjectAs();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path &path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path &path);
		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void OnDuplicateEntity();

		void UiToolbar();

	private:
		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		Engine::Ref<Engine::Scene> m_ActiveScene;
		Engine::Ref<Engine::Scene> m_EditorScene;

		std::filesystem::path m_EditorScenePath;

		bool m_PrivateCamera = true;

		Engine::EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.f, 0.f };
		glm::vec2 m_ViewportBounds[2];

		int m_GuizmoType = -1;

		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		Engine::Scope<ContentBrowserPanel> m_ContentBrowserPanel;
	};
}