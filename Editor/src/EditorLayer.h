#pragma once

#include "Engine.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/ToolbarPanel.h"

#include "Managers/DockspaceManager.h"
#include "SceneStateController.h"

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
		void SaveProject(const std::filesystem::path& path);
		void SaveProject();
		bool SaveProjectAs();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path &path);
		bool SaveScene(const std::filesystem::path &path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path &path);
		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void UpdateSceneState(SceneStateController::State state);

		void OnDuplicateEntity();
	private:
		std::filesystem::path m_EditorScenePath;

		bool m_PrivateCamera = true;

		Engine::EditorCamera m_EditorCamera;

		Engine::Scope<SceneContext> m_SceneContext;
		Engine::Scope<SceneStateController> m_SceneController;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		Engine::Scope<ContentBrowserPanel> m_ContentBrowserPanel;
		ViewportPanel m_ViewportPanel;
		Engine::Scope <ToolbarPanel> m_ToolbarPanel;

		DockspaceManager m_DockspaceManager;
	};
}