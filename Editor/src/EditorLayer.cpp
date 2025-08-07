#include "EditorLayer.h"

#include <Engine/Scene/SceneSerializer.h>
#include <Engine/Math/Math.h>
#include <Engine/Utils/FileDialogs.h>
#include <Engine/Project/Project.h>

#include <ImGui/ImGui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Editor {
	bool DrawButtonIcon(const Engine::Ref<Engine::Texture>& icon, const std::string& name, float size,
	                    const ImVec4& tinitColor) {
		return ImGui::ImageButton(name.c_str(), icon->RendererID(), {size, size}, {0.0f, 0.0f}, {1.0f, 1.0f},
		                          {0.0f, 0.0f, 0.0f, 0.0f}, tinitColor);
	}

	EditorLayer::EditorLayer() : Layer("EditorLayer") {
	}

	void EditorLayer::OnAttach() {
		Layer::OnAttach();

		m_SceneContext = Engine::MakeScope<SceneContext>();
		m_SceneController = Engine::MakeScope<SceneStateController>(m_SceneContext.get());

		m_SceneContext->New();

		if (Engine::Application::Get().GetSpecification().CommandLineArgs.Count > 1) {
			OpenProject(Engine::Application::Get().GetSpecification().CommandLineArgs.At(1));
		}
		else {
			NewProject();
			//if (!OpenProject())
			//	Engine::Application::Get().Close();
		}

		m_EditorCamera = Engine::EditorCamera(30.f, 1.778f, 0.1f, 1000.f);

		m_ViewportPanel.SetSceneContext(m_SceneContext.get());
		m_ViewportPanel.SetEditorCamera(&m_EditorCamera);

		m_ToolbarPanel = Engine::MakeScope<ToolbarPanel>();

		m_ToolbarPanel->SetCallbackPlay([this]() {OnScenePlay(); });
		m_ToolbarPanel->SetCallbackSimulate([this]() {OnSceneSimulate(); });
		m_ToolbarPanel->SetCallbackPause([this]() { OnScenePause(); });
		m_ToolbarPanel->SetCallbackStop([this]() {OnSceneStop(); });
		m_ToolbarPanel->SetCallbackStep([this]() {m_SceneContext->Step(); });
	}

	void EditorLayer::OnDetach() {
		Layer::OnDetach();
	}

	void EditorLayer::OnUpdate() {
		Layer::OnUpdate();

		m_ViewportPanel.OnUpdate();

		switch (m_SceneController->GetState()) {
		case SceneState::Edit: {
			m_EditorCamera.OnUpdate();
			m_SceneContext->GetActiveScene()->OnUpdateEditor(m_EditorCamera);
			break;
		}
		case SceneState::Simulate: {
			m_EditorCamera.OnUpdate();
			m_SceneContext->GetActiveScene()->OnUpdateSimulation(m_EditorCamera);
			break;
		}
		case SceneState::Play: {
			m_SceneContext->GetActiveScene()->OnUpdateRuntime();
			break;
		}
		}
	}

	void EditorLayer::OnConstUpdate(const Engine::Time& timeStep) {
		switch (m_SceneController->GetState()) {
		case SceneState::Edit: {
			m_SceneContext->GetActiveScene()->OnConstUpdateEditor(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Simulate: {
			m_SceneContext->GetActiveScene()->OnConstUpdateSimulation(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Play: {
			m_SceneContext->GetActiveScene()->OnConstUpdateRuntime(timeStep);
			break;
		}
		}
	}

	void EditorLayer::OnImGuiRender() {
		m_DockspaceManager.BeginDockspace();
		ImGuiStyle& style = ImGui::GetStyle();

		const float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Project"))
					NewProject();

				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
					NewScene();
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
					SaveScene();
				}

				if (ImGui::MenuItem("Save Scene As ...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Engine::Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		m_ViewportPanel.SetSelectedEntity(m_SceneHierarchyPanel.GetSelectedEntity());
		m_ViewportPanel.OnImGuiRender();

		m_ToolbarPanel->OnImGuiRender();

		m_DockspaceManager.EndDockspace();
	}

	void EditorLayer::OnEvent(Engine::Event& e) {
		Engine::EventDispatcher dispacher(e);
		dispacher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispacher.Dispatch<Engine::MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(Engine::KeyPressedEvent& e) {
		if (e.IsRepeat())
			return false;

		const bool shift = Engine::Keyboard::IsKeyPressed(Engine::Key::LeftShift) || Engine::Keyboard::IsKeyPressed(
			Engine::Key::RightShift);
		const bool control = Engine::Keyboard::IsKeyPressed(Engine::Key::LeftControl) || Engine::Keyboard::IsKeyPressed(
			Engine::Key::RightControl);

		switch (e.GetKeyCode()) {
		case Engine::Key::N:
			if (control)
				NewScene();
			break;

		case Engine::Key::O:
			if (control)
				OpenScene();
			break;

		case Engine::Key::S:
			if (control) {
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;

		case Engine::Key::D:
			if (control)
				OnDuplicateEntity();
			break;
		}

		m_ViewportPanel.OnKeyPressed(e);

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(Engine::MouseButtonPressedEvent& e) {
		return false;
	}

	void EditorLayer::NewProject() {
		Engine::ProjectConfig config;
		config.AssetDirectory = std::filesystem::current_path();
		config.ScriptModulePath = std::filesystem::current_path() / "Scripts";
		Engine::Project::New(config);
		m_ContentBrowserPanel = Engine::MakeScope<ContentBrowserPanel>();
	}

	bool EditorLayer::OpenProject() {
		std::filesystem::path filePath = Engine::FileDialogs::OpenFile("Game Project (*.gproj)\0*.gproj\0");

		if (filePath.empty())
			return false;

		OpenProject(filePath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path) {
		if (Engine::Project::Load(path)) {
			auto startScenePath = Engine::Project::GetAssetFileSystemPath(
				Engine::Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);
			m_ContentBrowserPanel = Engine::MakeScope<ContentBrowserPanel>();
		}
	}

	void EditorLayer::SaveProject(const std::filesystem::path& path) {
		Engine::Project::SaveActive(path);
		Engine::ProjectConfig config;
		config.AssetDirectory = path;
		config.ScriptModulePath = path / "Scripts";

		Engine::Project::New(config);
	}

	void EditorLayer::SaveProject() {
		Engine::Project::SaveActive(Engine::Project::GetProjectDirectory());
	}

	bool EditorLayer::SaveProjectAs() {
		std::filesystem::path filePath = Engine::FileDialogs::SaveFile("Game Project (*.gproj)\0*.gproj\0");

		if (filePath.empty())
			return false;

		SaveProject(filePath);
		return true;
	}

	void EditorLayer::NewScene() {
		m_SceneContext->New();
		m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());
	}

	void EditorLayer::OpenScene() {
		std::string filePath = Engine::FileDialogs::OpenFile("Game Scene (*.gscene)\0*.gscene\0");
		if (!filePath.empty())
			OpenScene(filePath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path) {
		if (m_SceneController->GetState() != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".gscene") {
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Engine::Ref<Engine::Scene> newScene = Engine::MakeRef<Engine::Scene>();
		Engine::SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path)) {
			m_SceneContext->Set(newScene);
			m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());

			m_EditorScenePath = path;
		}
	}

	bool EditorLayer::SaveScene(const std::filesystem::path& path) {
		if (!path.empty()) {
			m_EditorScenePath = path;
			SerializeScene(m_SceneContext->GetActiveScene(), path);
			return true;
		}

		return false;
	}

	void EditorLayer::SaveScene() {
		if (!SaveScene(m_EditorScenePath))
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs() {
		SaveScene(Engine::FileDialogs::SaveFile("Game Scene (*.game)\0*.game\0"));
	}

	void EditorLayer::SerializeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path& path) {
		Engine::SceneSerializer serializer(scene);
		serializer.Serialize(path);
	}

	void EditorLayer::OnScenePlay() {
		m_SceneController->Play();

		UpdateSceneState(SceneState::Play);
	}

	void EditorLayer::OnSceneSimulate() {
		m_SceneController->Simulate();

		UpdateSceneState(SceneState::Simulate);
	}

	void EditorLayer::OnSceneStop() {
		m_SceneController->Stop();

		UpdateSceneState(SceneState::Edit);
	}

	void EditorLayer::OnScenePause() {
		if (m_SceneController->GetState() == SceneState::Edit)
			return;

		const bool paused = !m_SceneContext->IsPaused();

		if (m_SceneController->GetState() != SceneState::Edit) {
			m_SceneContext->SetPaused(paused);
			m_ToolbarPanel->SetPaused(paused);
		}
	}

	void EditorLayer::UpdateSceneState(SceneState state) {
		if (m_SceneController->GetState() == state)
			return;

		m_ToolbarPanel->SetState(state);
		m_ToolbarPanel->SetPaused(false);
		m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());
	}

	void EditorLayer::OnDuplicateEntity() {
		if (m_SceneController->GetState() != SceneState::Edit)
			return;

		const Engine::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity) {
			Engine::Entity newEntity = m_SceneContext->GetEditorScene()->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}
}
