#include "pch.h"
#include "Engine/Renderer/EditorCamera.h"

#include "Engine/Core/KeyCode.h"
#include "Engine/Core/MouseCodes.h"

#include "Engine/Devices/Mouse.h"
#include "Engine/Devices/Keyboard.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine {
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip) : Camera(
			 glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)
			),
		m_FOV(fov),
		m_AspectRatio(aspectRatio),
		m_NearClip(nearClip),
		m_FarClip(farClip) {
		UpdateView();
	}

	glm::vec3 EditorCamera::GetUpDirection() const {
		return glm::rotate(GetOrientation(), glm::vec3(0.f, 1.f, 0.f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const {
		return glm::rotate(GetOrientation(), glm::vec3(1.f, 0.f, 0.f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const {
		return glm::rotate(GetOrientation(), glm::vec3(0.f, 0.f, -1.f));
	}

	glm::quat EditorCamera::GetOrientation() const {
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.f));
	}

	void EditorCamera::UpdateProjection() {
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);

		UpdateViewProjectionMatrix();
	}

	void EditorCamera::UpdateView() {
		m_Position = CalculatePosition();

		const glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		UpdateViewProjectionMatrix();
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
		const float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();

		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta) {
		const auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta) {
		const float yawSig = GetUpDirection().y < 0 ? -1.f : 1.f;
		m_Yaw += yawSig * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta) {
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.f) {
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.f;
		}
	}

	glm::vec3 EditorCamera::CalculatePosition() const {
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	std::pair<float, float> EditorCamera::PanSpeed() const {
		const float x = std::min(m_ViewportWidth / 1000.f, 2.4f);
		const float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float y = std::min(m_ViewportHeight / 1000.f, 2.4f);
		const float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return {xFactor, yFactor};
	}

	float EditorCamera::RotationSpeed() const {
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const {
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.f);

		float speed = distance * distance;
		speed = std::min(speed, 1000.f);

		return speed;
	}

	void EditorCamera::OnUpdate(Time timeStep) {
		if (Keyboard::IsKeyPressed(Key::LeftAlt)) {
			const glm::vec2& mouse = { Mouse::GetX(), Mouse::GetY() };
			const glm::vec2 delta = ( mouse - m_InitialMousePosition ) *0.003f;
			m_InitialMousePosition = mouse;

			if (Mouse::IsButtonPressed(MouseButton::ButtonMiddle))
				MousePan(delta);
			else if (Mouse::IsButtonPressed(MouseButton::ButtonLeft))
				MouseRotate(delta);
			else if (Mouse::IsButtonPressed(MouseButton::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}
}
