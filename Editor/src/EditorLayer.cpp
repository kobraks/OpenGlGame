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

	void EditorLayer::OnEvent(Event &e)
	{
		Layer::OnEvent(e);

		EventDispatcher dispacher(e);
		dispacher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispacher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent &e)
	{
		if (e.IsRepeat())
			return false;

		const bool shift = Keyboard::IsKeyPressed(Key::LeftShift) || Keyboard::IsKeyPressed(Key::RightShift);
		const bool control = Keyboard::IsKeyPressed(Key::LeftControl) || Keyboard::IsKeyPressed(Key::RightControl);

		switch(e.GetKeyCode())
		{
			case Key::N:
				if (control)
					NewScene();
			break;

			case Key::O:
				if (control)
					OpenScene();
			break;

			case Key::S:
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
			break;

			case Key::D:
				if (control)
					OnDuplicateEntity();
			break;

			case Key::Q:
				if (!ImGuizmo::IsUsing())
					m_GuizmoType = -1;
			break;

			case Key::W:
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;

			case Key::E:
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
			break;

			case Key::R:
				if(!ImGuizmo::IsUsing())
					m_GuizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
	{
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		// m_ActiveScene->OnComponentAdded();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene() {}
	void EditorLayer::OpenScene(const std::filesystem::path &path) {}
	void EditorLayer::SaveScene() {}
	void EditorLayer::SaveSceneAs() {}
	void EditorLayer::SeralizeScene(Ref<Scene> scene, const std::filesystem::path &path) {}
	void EditorLayer::OnDuplicateEntity() {}
	void EditorLayer::UiToolbar() {}
}
