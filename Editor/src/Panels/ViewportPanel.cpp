
#include "ViewportPanel.h"

#include "Engine/Core/Application.h"

#include "Engine/Devices/Keyboard.h"

#include "Engine/Renderer/RendererCommand.h"
#include "Engine/Renderer/Builders/TextureAttachmentBuilder.h"
#include "Engine/Renderer/Builders/FramebufferBuilder.h"

#include "Engine/Math/Math.h"

#include "../Managers/SceneManager.h" // TODO fix include path

#include <ImGui/imgui.h>
#include "ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

namespace Editor {
	ViewportPanel::ViewportPanel() {
		const auto depthTexture = Engine::TextureAttachmentBuilder().Format(Engine::ImageFormat::Depth24Stencil8).Label("DepthStencilTestViewport");
		const auto colorBuffer = Engine::TextureAttachmentBuilder().Format(Engine::ImageFormat::RGBA8).Label("ColorBufferViewport");
		const auto pickerBuffer = Engine::TextureAttachmentBuilder().Format(Engine::ImageFormat::R32I).Label("PickerBufferViewport");

		m_Framebuffer = Engine::FramebufferBuilder().AddColorAttachment(colorBuffer).
			AddColorAttachment(pickerBuffer).
			DepthAttachment(depthTexture).
			Label("Viewport").
			Build();
	}

	void ViewportPanel::OnImGuiRender() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		const auto minRegion = ImGui::GetWindowContentRegionMin();
		const auto maxRegion = ImGui::GetWindowContentRegionMax();
		const auto offset = ImGui::GetWindowPos();

		m_Bounds[0] = { minRegion.x + offset.x, minRegion.y + offset.y };
		m_Bounds[1] = { maxRegion.x + offset.x, maxRegion.y + offset.y };

		m_Focused = ImGui::IsWindowFocused();
		m_Hovered = ImGui::IsWindowHovered();

		Engine::Application::Get().GetImGuiLayer()->BlockEvents(!m_Hovered);

		const ImVec2 panelSize = ImGui::GetContentRegionAvail();
		m_Size = { panelSize.x, panelSize.y };

		const auto texture = m_Framebuffer->GetColorTextureAttachment();
		ImGui::Image(texture->RendererID(), ImVec2{ m_Size.x, m_Size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				auto path = static_cast<const wchar_t*>(payload->Data);

				//TODO: Handle the dropped file
			}

			ImGui::EndDragDropTarget();
		}

		HandleGuizmo();

		ImGui::End(); //End viewport
		ImGui::PopStyleVar();
	}

	void ViewportPanel::OnUpdate() {
		m_SceneManager->GetActiveScene()->OnViewportResize(static_cast<int32_t>(m_Size.x), static_cast<int32_t>(m_Size.y));

		if (const auto size = m_Framebuffer->Size(); m_Size.x > 0.f && m_Size.y > 0.f && (size.Width != static_cast<uint32_t>(m_Size.x) || size.Height != static_cast<uint32_t>(m_Size.y))) {
			m_Framebuffer->Resize({ static_cast<uint32_t>(m_Size.x), static_cast<uint32_t>(m_Size.y) });
			m_EditorCamera->SetViewportSize(m_Size.x, m_Size.y);
		}

		m_Framebuffer->Bind();
		Engine::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		Engine::RendererCommand::Clear();

		m_Framebuffer->GetColorTextureAttachment()->Clear(-1);


		//picking
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_Bounds[0].x;
		my -= m_Bounds[0].y;

		const glm::vec2 viewportSize = m_Bounds[1] - m_Bounds[0];
		my = viewportSize.y - my;

		const int32_t mouseX = static_cast<int32_t>(mx);
		const int32_t mouseY = static_cast<int32_t>(my);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int32_t>(viewportSize.x) && mouseY < static_cast<int32_t>(viewportSize.y)) {
			const int32_t pixelData = m_Framebuffer->ReadPixel(1, { mouseX, mouseY });
			m_HoveredEntity = pixelData == -1 ? Engine::Entity() : Engine::Entity(static_cast<entt::entity>(pixelData), m_SceneManager->GetActiveScene().get());
		}

		m_Framebuffer->Unbind();
	}

	void ViewportPanel::OnKeyPressed(Engine::KeyPressedEvent& e) {
		switch (e.GetKeyCode()) {
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
	}

	void ViewportPanel::HandleGuizmo() {
		Engine::Entity selectedEntity = m_SelectedEntity;

		if (selectedEntity && m_GuizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_Bounds[0].x, m_Bounds[0].y,
				m_Bounds[1].x - m_Bounds[0].x,
				m_Bounds[1].y - m_Bounds[0].y);

			//Camera
			//
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<Engine::CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjectionMatrix();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<Engine::TransformComponent>().GetTransform());

			//Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera->GetProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera->GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<Engine::TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			const bool snap = Engine::Keyboard::IsKeyPressed(Engine::Key::LeftControl);
			float snapValue = 0.5f;

			if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			Manipulate(value_ptr(cameraView), value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GuizmoType), ImGuizmo::LOCAL, value_ptr(transform), nullptr,
				snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Engine::Math::DecomposeTransform(transform, translation, rotation, scale);

				const glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
	}
}
