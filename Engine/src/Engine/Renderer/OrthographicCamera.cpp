#include "pch.h"
#include "Engine/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : Camera(glm::ortho(left, right, bottom, top, -1.f, -1.f)){
		UpdateViewProjectionMatrix();
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, -1.f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix() {
		const glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position) * glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		UpdateViewProjectionMatrix();
	}
}