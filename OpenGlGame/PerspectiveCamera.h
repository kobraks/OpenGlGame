#pragma once

#include "Camera.h"

namespace Game
{
	class PerspectiveCamera: public Camera
	{
		float m_Fov = 0.f;
		float m_AspectRatio = 0.f;
		float m_NearPlane = 0.f;
		float m_FarPlane = 0.f;

		glm::vec3 m_Position = glm::vec3(0.f);
		
		glm::vec3 m_Up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 m_Front = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 m_Right = glm::vec3(0.f);
		glm::vec3 m_WordUp = glm::vec3(0.f, 1.f, 0.f);

		glm::vec3 m_Rotation = {0.f, -90.f, 0.f};
	public:
		PerspectiveCamera(
			float fov,
			float aspectRatio,
			float nearPlane,
			float farPlane,
			glm::vec3 position,
			glm::vec3 front,
			glm::vec3 up = glm::vec3(0.f, 1.f, 0.f)
			);

		void SetFov(float fov);
		void SetAspectRatio(float aspectRatio);
		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);

		void SetRotation(glm::vec3 rotation);
		void Rotate(glm::vec3 rotate);

		void Move(glm::vec3 vector);
		
		void SetPosition(const glm::vec3 &position);
		void SetUp(const glm::vec3 &up);

		float GetFov() const { return m_Fov; }
		float GetAspectRatio() const { return m_AspectRatio; }
		float GetNearPlane() const { return m_NearPlane; }
		float GetFarPlane() const { return m_FarPlane; }

		glm::vec3 GetRight() const { return m_Right; }
		glm::vec3 GetFront() const { return m_Front; }
		glm::vec3 GetUp() const { return m_Up; }

		glm::vec3 GetPosition() const {return m_Position; }
		glm::vec3 GetWordUp() const { return m_WordUp; }

	private:
		void UpdateCameraVectors();
		void UpdateProjection();
		void UpdateView();
	};
}
