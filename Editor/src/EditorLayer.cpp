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

		m_IconPlay = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/PlayButton.png"));
		m_IconPause = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/PauseButton.png"));
		m_IconStep = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/StepButton.png"));
		m_IconSimulate = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/SimulateButton.png"));
		m_IconStop = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/StopButton.png"));
	}

	void EditorLayer::OnDetach() {
		Layer::OnDetach();
	}

	void EditorLayer::OnUpdate() {
		Layer::OnUpdate();

		m_ViewportPanel.OnUpdate();

		switch (m_SceneController->GetState()) {
		case SceneStateController::State::Edit: {
			m_EditorCamera.OnUpdate();
			m_SceneContext->GetActiveScene()->OnUpdateEditor(m_EditorCamera);
			break;
		}
		case SceneStateController::State::Simulate: {
			m_EditorCamera.OnUpdate();
			m_SceneContext->GetActiveScene()->OnUpdateSimulation(m_EditorCamera);
			break;
		}
		case SceneStateController::State::Play: {
			m_SceneContext->GetActiveScene()->OnUpdateRuntime();
			break;
		}
		}
	}

	void EditorLayer::OnConstUpdate(const Engine::Time& timeStep) {
		switch (m_SceneController->GetState()) {
		case SceneStateController::State::Edit: {
			m_SceneContext->GetActiveScene()->OnConstUpdateEditor(timeStep, m_EditorCamera);
			break;
		}
		case SceneStateController::State::Simulate: {
			m_SceneContext->GetActiveScene()->OnConstUpdateSimulation(timeStep, m_EditorCamera);
			break;
		}
		case SceneStateController::State::Play: {
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

		UiToolbar();

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
		if (m_SceneController->GetState() != SceneStateController::State::Edit)
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

		m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());
	}

	void EditorLayer::OnSceneSimulate() {
		m_SceneController->Simulate();

		m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());
	}

	void EditorLayer::OnSceneStop() {
		m_SceneController->Stop();

		m_SceneHierarchyPanel.SetContext(m_SceneContext->GetActiveScene());
	}

	void EditorLayer::OnScenePause() {
		m_SceneController->Pause();
	}

	void EditorLayer::OnDuplicateEntity() {
		if (m_SceneController->GetState() != SceneStateController::State::Edit)
			return;

		const Engine::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity) {
			Engine::Entity newEntity = m_SceneContext->GetEditorScene()->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::UiToolbar() {
		const auto state = m_SceneController->GetState();

		const bool hasPlayButton = state == SceneStateController::State::Edit || state == SceneStateController::State::Play;
		const bool hasSimulateButton = state == SceneStateController::State::Edit || state == SceneStateController::State::Simulate;
		const bool hasPauseButton = state != SceneStateController::State::Edit;

		const int buttonCount = hasPlayButton + hasSimulateButton + hasPauseButton + 1;

		constexpr float windowHeight = 32;

		constexpr float spacing = 4.0f;
		constexpr float buttonSize = windowHeight - spacing;

		const float windowWidth = buttonSize * static_cast<float>(buttonCount) + static_cast<float>(buttonCount - 1) *
			spacing;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		const auto& colors = ImGui::GetStyle().Colors;

		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::SetNextWindowSize({windowWidth, windowHeight + spacing * 2});
		// ImGui::SetNextWindowPos();
		ImGui::Begin("##toolbar", nullptr,
		             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = static_cast<bool>(m_SceneContext->GetActiveScene());

		auto tintColor = ImVec4(1, 1, 1, 1.f);

		if (!toolbarEnabled)
			tintColor.w = 0.5f;


		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.2f) - (buttonSize * 0.5f));

		if (hasPlayButton) {
			const auto& icon = (state == SceneStateController::State::Edit || state == SceneStateController::State::Simulate)
				                   ? m_IconPlay
				                   : m_IconStop;
			if (DrawButtonIcon(icon, "##PlayButton", buttonSize, tintColor) && toolbarEnabled) {
				if (state == SceneStateController::State::Edit || state == SceneStateController::State::Simulate)
					OnScenePlay();
				else if (state == SceneStateController::State::Play)
					OnSceneStop();
			}
		}

		if (hasSimulateButton) {
			if (hasPlayButton)
				ImGui::SameLine(0, spacing);

			const auto& icon = (state == SceneStateController::State::Edit || state == SceneStateController::State::Play)
				                   ? m_IconSimulate
				                   : m_IconStop;
			if (DrawButtonIcon(icon, "##SimulateButton", buttonSize, tintColor) && toolbarEnabled) {
				if (state == SceneStateController::State::Edit || state == SceneStateController::State::Play)
					OnSceneSimulate();
				else if (state == SceneStateController::State::Simulate)
					OnSceneStop();
			}
		}

		if (hasPauseButton) {
			const bool isPaused = m_SceneContext->IsPaused();
			ImGui::SameLine(0, spacing);
			{
				const auto& icon = m_IconPause;
				if (DrawButtonIcon(icon, "##PauseButton", buttonSize, tintColor) && toolbarEnabled) {
					m_SceneContext->SetPaused(!isPaused);
				}
			}

			if (isPaused) {
				ImGui::SameLine(0, spacing);
				{
					const auto& icon = m_IconStep;
					if (DrawButtonIcon(icon, "##StepButton", buttonSize, tintColor) && toolbarEnabled) {
						m_SceneContext->Step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}
