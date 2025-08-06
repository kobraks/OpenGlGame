#pragma once

#include <Engine/Renderer/Framebuffer.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Renderer/EditorCamera.h>
#include <glm/glm.hpp>

#include "Engine/Events/KeyEvent.h"

namespace Editor {
	class SceneContext;

	class ViewportPanel {
	public:
		ViewportPanel();

		void SetSceneContext(SceneContext* context) { m_Context = context; }
		void SetEditorCamera(Engine::EditorCamera* camera) { m_EditorCamera = camera; }

		SceneContext* GetSceneContext() const { return m_Context; }

		Engine::Ref<Engine::Framebuffer> GetFramebuffer() const { return m_Framebuffer; }
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
		SceneContext* m_Context = nullptr;
		Engine::EditorCamera* m_EditorCamera = nullptr;

		Engine::Ref<Engine::Framebuffer> m_Framebuffer = nullptr;

		glm::vec2 m_Size = { 0.f, 0.f };
		glm::vec2 m_Bounds[2];

		bool m_Focused = false;
		bool m_Hovered = false;

		int m_GuizmoType = -1; // -1 means no guizmo
		Engine::Entity m_HoveredEntity;
		Engine::Entity m_SelectedEntity;
	};
}
