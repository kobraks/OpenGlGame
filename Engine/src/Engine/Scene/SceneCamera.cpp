#include "pch.h"
#include "Engine/Scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	SceneCamera::SceneCamera() {
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip) {
		m_ProjectionType = ProjectionType::Perspective;

		m_PerspectiveFov = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
		m_ProjectionType = ProjectionType::Orthographic;

		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
		if (height == 0)
			m_AspectRatio = 0.f;
		else
			m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);

		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection() {
		if (m_ProjectionType == ProjectionType::Perspective)
			m_Projection = glm::perspective(m_PerspectiveFov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		else {
			const float left = -m_OrthographicSize * m_AspectRatio * 0.5f;
			const float right= m_OrthographicSize * m_AspectRatio * 0.5f;
			const float bottom = -m_OrthographicSize * 0.5f;
			const float top = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(left, right, bottom, top, m_OrthographicNear, m_OrthographicFar);
		}
	}

}