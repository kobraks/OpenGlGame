#include "EditorLayer.h"

#include <Engine/Scene/SceneSerializer.h>
#include <Engine/Math/Math.h>
#include <Engine/Renderer/RendererCommand.h>
#include <Engine/Utils/FileDialogs.h>
#include <Engine/Project/Project.h>

#include <ImGui/ImGui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuizmo.h"

namespace Editor {
	EditorLayer::EditorLayer() : Layer("EditorLayer") {
	}

	void EditorLayer::OnAttach() {
		Layer::OnAttach();

		m_EditorScene = Engine::MakeRef<Engine::Scene>();
		m_ActiveScene = m_EditorScene;

		if (Engine::Application::Get().GetCommandLineArgCount() > 1) {
			OpenProject(Engine::Application::Get().GetCommandLineArg(1));
		}
		else {
			NewProject();
			//if (!OpenProject())
			//	Engine::Application::Get().Close();
		}

		m_EditorCamera = Engine::EditorCamera(30.f, 1.778f, 0.1f, 1000.f);
	}

	void EditorLayer::OnDetach() {
		Layer::OnDetach();
	}

	void EditorLayer::OnUpdate() {
		Layer::OnUpdate();

		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x),
		                                static_cast<uint32_t>(m_ViewportSize.y));

		Engine::RendererCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
		Engine::RendererCommand::Clear();

		switch (m_SceneState) {
		case SceneState::Edit: {
			m_EditorCamera.OnUpdate();
			m_ActiveScene->OnUpdateEditor(m_EditorCamera);
			break;
		}
		case SceneState::Simulate: {
			m_EditorCamera.OnUpdate();
			m_ActiveScene->OnUpdateSimulation(m_EditorCamera);
			break;
		}
		case SceneState::Play: {
			m_ActiveScene->OnUpdateRuntime();
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y;

		int mouseX = static_cast<int>(mx);
		int mouseY = static_cast<int>(my);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y)) {
		}

	}

	void EditorLayer::OnConstUpdate(const Engine::Time& timeStep) {
		switch (m_SceneState) {
		case SceneState::Edit: {
			m_ActiveScene->OnConstUpdateEditor(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Simulate: {
			m_ActiveScene->OnConstUpdateSimulation(timeStep, m_EditorCamera);
			break;
		}
		case SceneState::Play: {
			m_ActiveScene->OnConstUpdateRuntime(timeStep);
			break;
		}
		}
	}

	void EditorLayer::OnImGuiRender() {
		static bool dockspaceOpen = true;
		static bool optFullscreenPersistant = true;
		bool optFullscreen = optFullscreenPersistant;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (optFullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar();

		if (optFullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		const float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			const ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), dockspaceFlags);
		}

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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
		m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Engine::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& e) {
		Layer::OnEvent(e);

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

		case Engine::Key::Q:
			if (!ImGuizmo::IsUsing())
				m_GuizmoType = -1;
			break;

		case Engine::Key::W:
			if (!ImGuizmo::IsUsing())
				m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

		case Engine::Key::E:
			if (!ImGuizmo::IsUsing())
				m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
			break;

		case Engine::Key::R:
			if (!ImGuizmo::IsUsing())
				m_GuizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

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

	void EditorLayer::SaveProject() {
		// if (Engine::Project::SaveActive())
	}

	void EditorLayer::SaveProjectAs() {
		
	}

	void EditorLayer::NewScene() {
		m_ActiveScene = Engine::MakeRef<Engine::Scene>();
		// m_ActiveScene->OnComponentAdded();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene() {
		std::string filePath = Engine::FileDialogs::OpenFile("Game Scene (*.gscene)\0*.gscene\0");
		if (!filePath.empty())
			OpenScene(filePath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path) {
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".gscene") {
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Engine::Ref<Engine::Scene> newScene = Engine::MakeRef<Engine::Scene>();
		Engine::SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path)) {
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene() {
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs() {
		std::string filePath = Engine::FileDialogs::SaveFile("Game Scene (*.game)\0*.game\0");
		if (!filePath.empty()) {
			SerializeScene(m_ActiveScene, filePath);
			m_EditorScenePath = filePath;
		}
	}

	void EditorLayer::SerializeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path& path) {
	}

	void EditorLayer::OnScenePlay() {
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate() {
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Engine::Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop() {
		GAME_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause() {
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

	void EditorLayer::OnDuplicateEntity() {
		if (m_SceneState != SceneState::Edit)
			return;

		const Engine::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity) {
			Engine::Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::UiToolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		auto& colors = ImGui::GetStyle().Colors;

		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr,
		             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		{
			const bool toolbarEnabled = static_cast<bool>(m_ActiveScene);

			auto tintColor = ImVec4(1, 1, 1, 1);
			if (!toolbarEnabled)
				tintColor.w = 0.5f;

			const float size = ImGui::GetWindowHeight() - 4.f;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

			const bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
			const bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
			const bool hasPauseButton = m_SceneState != SceneState::Edit;

			if (hasPlayButton) {
			}

			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(3);
		}

		ImGui::End();
	}
}
