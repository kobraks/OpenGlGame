#include "pch.h"
#include "Engine/Renderer/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio) : Camera(
		glm::perspective(
			fov, aspectRatio, 0.f, 1.f)) {
		UpdateViewProjectionMatrix();
	}

	void PerspectiveCamera::SetProjection(float fov, float aspectRatio) {
		m_ProjectionMatrix = glm::perspective(fov, aspectRatio, 0.f, 1.f);
		UpdateViewProjectionMatrix();
	}

	void PerspectiveCamera::SetPosition(const glm::vec3& position) {
		m_Position = position;
		UpdateCameraVectors();
		RecalculateViewMatrix();
	}

	const glm::vec3& PerspectiveCamera::GetPosition() const { return m_Position; }

	void PerspectiveCamera::SetTarget(const glm::vec3& target) {
		m_Target = target;
		UpdateCameraVectors();
		RecalculateViewMatrix();
	}

	const glm::vec3& PerspectiveCamera::GetTarget() const { return m_Target; }

	void PerspectiveCamera::SetRotation(const glm::vec3& rotation) {
		m_Rotation = rotation;
		UpdateCameraVectors();
		RecalculateViewMatrix();
	}

	const glm::vec3& PerspectiveCamera::GetRotation() const { return m_Rotation; }

	void PerspectiveCamera::RecalculateViewMatrix() {
		m_ViewMatrix = lookAt(m_Position, m_Position + m_Front, m_UP);
		UpdateViewProjectionMatrix();
	}

	void PerspectiveCamera::UpdateCameraVectors() {
		const float pitch = glm::radians(m_Rotation.x);
		const float yaw = glm::radians(m_Rotation.y);
		const float roll = glm::radians(m_Rotation.z);

		glm::vec3 front;

		front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
		front.y = sinf(glm::radians(pitch));
		front.z = sinf(yaw) * cosf(pitch);

		m_Front = normalize(front);
		m_Right = normalize(cross(m_Front, m_WorldUp));

		const glm::mat4 rollMat = rotate(glm::mat4(1.f), roll, m_Front);
		m_UP = normalize(cross(m_Right, m_Front));
		m_UP = glm::mat3(rollMat) * m_UP;
	}
}
