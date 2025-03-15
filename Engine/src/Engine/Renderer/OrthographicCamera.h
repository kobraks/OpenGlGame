#pragma once

#include "Engine/Core/Base.h"

#include "Engine/Renderer/Camera.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Engine {
	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position;  }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		float GetRotation() const { return m_Rotation;  }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

	private:
		void RecalculateViewMatrix();

	private:
		glm::vec3 m_Position = { 0.f, 0.f, 0.f };
		float m_Rotation = 0.f;
	};
}