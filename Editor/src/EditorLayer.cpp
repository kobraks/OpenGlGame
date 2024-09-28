#include "EditorLayer.h"

#include <Engine/Scene/SceneSerializer.h>
#include <Engine/Math/Math.h>

#include <ImGui/ImGui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuizmo.h"

namespace Game
{
	EditorLayer::EditorLayer() : Layer("EditorLayer") {}

	void EditorLayer::OnAttach()
	{
		Layer::OnAttach();
	}

	void EditorLayer::OnDetach()
	{
		Layer::OnDetach();
	}

	void EditorLayer::OnUpdate()
	{
		Layer::OnUpdate();
	}

	void EditorLayer::OnImGuiRender()
	{
		Layer::OnImGuiRender();
	}

	void EditorLayer::OnEvent(Engine::Event &e)
	{
		Layer::OnEvent(e);

		Engine::EventDispatcher dispacher(e);
		dispacher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispacher.Dispatch<Engine::MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(Engine::KeyPressedEvent &e)
	{
		if (e.IsRepeat())
			return false;

		const bool shift   = Engine::Keyboard::IsKeyPressed(Engine::Key::LeftShift) || Engine::Keyboard::IsKeyPressed(Engine::Key::RightShift);
		const bool control = Engine::Keyboard::IsKeyPressed(Engine::Key::LeftControl) || Engine::Keyboard::IsKeyPressed(Engine::Key::RightControl);

		switch(e.GetKeyCode())
		{
			case Engine::Key::N:
				if (control)
					NewScene();
			break;

			case Engine::Key::O:
				if (control)
					OpenScene();
			break;

			case Engine::Key::S:
				if (control)
				{
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
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

			case Engine::Key::E:
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
			break;

			case Engine::Key::R:
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(Engine::MouseButtonPressedEvent &e)
	{
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = Engine::MakeRef<Engine::Scene>();
		// m_ActiveScene->OnComponentAdded();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene() {}
	void EditorLayer::OpenScene(const std::filesystem::path &path) {}
	void EditorLayer::SaveScene() {}
	void EditorLayer::SaveSceneAs() {}
	void EditorLayer::SeralizeScene(Engine::Ref<Engine::Scene> scene, const std::filesystem::path &path) {}
	void EditorLayer::OnDuplicateEntity() {}
	void EditorLayer::UiToolbar() {}
}
