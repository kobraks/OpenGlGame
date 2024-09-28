#pragma once

#include "Engine.h"

#include "Panels/SceneHierarchyPanel.h"

namespace Game
{
	class EditorLayer : public Engine::Layer
	{
	private:
		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		Engine::Ref<Engine::Scene> m_ActiveScene;
		Engine::Ref<Engine::Scene> m_EditorScene;

		int m_GuizmoType = -1;

		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate() override;

		void OnImGuiRender() override;
		void OnEvent(Engine::Event &e) override;

	private:
		bool OnKeyPressed(Engine::KeyPressedEvent &e);
		bool OnMouseButtonPressed(Engine::MouseButtonPressedEvent &e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path &path);
		void SaveScene();;
		void SaveSceneAs();

		void SeralizeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path &path);

		void OnDuplicateEntity();

		void UiToolbar();
	};
}