#pragma once
#include "Engine/Core/Base.h"

#include <glm/mat4x4.hpp>

namespace Engine {
	class Camera {
	public:
		Camera() = default;
		explicit Camera(const glm::mat4 &projection) : m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetViewProjectionMatrix() const{ return m_ViewProjectionMatrix; }
		const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix;  }
	protected:
		void UpdateViewProjectionMatrix() { m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix; }

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.f);
	};
}