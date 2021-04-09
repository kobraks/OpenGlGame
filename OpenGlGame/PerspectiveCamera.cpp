#include "pch.h"
#include "PerspectiveCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Game
{
	PerspectiveCamera::PerspectiveCamera(
		float fov,
		float aspectRatio,
		float nearPlane,
		float farPlane,
		glm::vec3 position,
		glm::vec3 front,
		glm::vec3 up
		) : m_Fov(fov),
		    m_AspectRatio(aspectRatio),
		    m_NearPlane(nearPlane),
		    m_FarPlane(farPlane),
		    m_Position(position),
		    m_Front(front),
		    m_WordUp(up)
	{
		UpdateProjection();
		UpdateCameraVectors();
	}

	void PerspectiveCamera::SetFov(float fov)
	{
		m_Fov = fov;
		UpdateProjection();
	}
	
	void PerspectiveCamera::SetAspectRatio(float aspectRatio)
	{
		m_AspectRatio = aspectRatio;
		UpdateProjection();
	}
	
	void PerspectiveCamera::SetNearPlane(float nearPlane)
	{
		m_NearPlane = nearPlane;
		UpdateProjection();
	}
	
	void PerspectiveCamera::SetFarPlane(float farPlane)
	{
		m_FarPlane = farPlane;
		UpdateProjection();
	}

	void PerspectiveCamera::SetRotation(glm::vec3 rotation)
	{
		m_Rotation = rotation;
		UpdateCameraVectors();
	}
	
	void PerspectiveCamera::Rotate(glm::vec3 rotate)
	{
		m_Rotation += rotate;

		m_Rotation.x = std::clamp(m_Rotation.x, -89.f, 90.f);
		
		UpdateCameraVectors();
	}
	void PerspectiveCamera::Move(glm::vec3 vector)
	{
		m_Position += vector;
		UpdateCameraVectors();
	}

	void PerspectiveCamera::SetPosition(const glm::vec3 &position)
	{
		m_Position = position;
		UpdateCameraVectors();
	}
	
	void PerspectiveCamera::SetUp(const glm::vec3 &up)
	{
		m_WordUp = up;
		UpdateCameraVectors();
	}

	void PerspectiveCamera::UpdateCameraVectors()
	{
		glm::vec3 front;

		front.x = cosf(glm::radians(m_Rotation.y)) * cosf(glm::radians(m_Rotation.x));
		front.y = sinf(glm::radians(m_Rotation.x));
		front.z = sinf(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WordUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
		
		UpdateView();
	}

	void PerspectiveCamera::UpdateProjection()
	{
		SetProjection(glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane));
	}
	
	void PerspectiveCamera::UpdateView()
	{
		SetView(glm::lookAt(m_Position, m_Position + m_Front, m_Up));
	}
}
