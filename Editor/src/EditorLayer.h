#pragma once

#include "Engine.h"

#include "Panels/SceneHierarchyPanel.h"

namespace Game
{
	class EditorLayer : public Layer
	{
	private:
		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;

		int m_GuizmoType = -1;

		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate() override;

		void OnImGuiRender() override;
		void OnEvent(Event &e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent &e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path &path);
		void SaveScene();;
		void SaveSceneAs();

		void SeralizeScene(Ref<Scene> scene, const std::filesystem::path &path);

		void OnDuplicateEntity();

		void UiToolbar();
	};
}