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

	void EditorCamera::UpdateProjection() {
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const {
		float x = std::min(m_ViewportWidth / 1000.f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

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
			
		}
	}





}
