#pragma once

#include <Engine/Renderer/Framebuffer.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Renderer/EditorCamera.h>
#include <glm/glm.hpp>

#include "Engine/Events/KeyEvent.h"

namespace Editor {
	class ViewportPanel {
	public:
		ViewportPanel();

		void SetScene(const Engine::Ref<Engine::Scene>& scene);
		void SetEditorCamera(Engine::EditorCamera* camera);

		Engine::Ref<Engine::Framebuffer> GetFramebuffer() const { return m_Framebuffer; }
		Engine::Ref<Engine::Scene> GetScene() const { return m_Scene; }
		Engine::EditorCamera* GetEditorCamera() const { return m_EditorCamera; }

		void SetSelectedEntity(Engine::Entity entity) { m_SelectedEntity = entity; }

		void OnImGuiRender();
		void OnUpdate();

		void OnKeyPressed(Engine::KeyPressedEvent& e);

		bool IsFocused() const { return m_Focused; }
		bool IsHovered() const { return m_Hovered; }

		glm::vec2 GetSize() const { return m_Size; }
		glm::vec2* GetBounds() { return m_Bounds; }

		Engine::Entity GetHoveredEntity() const { return m_HoveredEntity; }

	private:
		void HandleGuizmo();

	private:
		Engine::Ref<Engine::Scene> m_Scene;
		Engine::EditorCamera* m_EditorCamera = nullptr;
		Engine::Ref<Engine::Framebuffer> m_Framebuffer;

		glm::vec2 m_Size = { 0.f, 0.f };
		glm::vec2 m_Bounds[2];

		bool m_Focused = false;
		bool m_Hovered = false;

		int m_GuizmoType = -1; // -1 means no guizmo
		Engine::Entity m_HoveredEntity;
		Engine::Entity m_SelectedEntity;
	};
}
