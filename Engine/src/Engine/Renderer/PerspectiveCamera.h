#pragma once

#include "Engine/Core/Base.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


namespace Engine {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(float fov, float aspectRatio);

		void SetProjection(float fov, float aspectRatio);

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const;

		void SetTarget(const glm::vec3& target);
		const glm::vec3& GetTarget() const;

		void SetRotation(const glm::vec3& rotation);
		const glm::vec3& GetRotation() const;

	private:
		void RecalculateViewMatrix();
		void UpdateCameraVectors();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		const glm::vec3 m_WorldUp = { 0.f, 1.f, 0.f };

		glm::vec3 m_Rotation = { 0.f, 0.f, 0.f };
		glm::vec3 m_Position = { 0.f, 0.f, 0.f };
		glm::vec3 m_Target = { 0.f, 0.f , 0.f };

		glm::vec3 m_UP = { 0.f, 1.f, 0.f };
		glm::vec3 m_Front = { 0.f, 0.f, -1.f };
		glm::vec3 m_Right = { 0.f, 0.f, 0.f };
	};
}