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
		glm::vec3 target,
		glm::vec3 up
		) : m_Fov(fov),
		    m_AspectRatio(aspectRatio),
		    m_NearPlane(nearPlane),
		    m_FarPlane(farPlane),
		    m_Position(position),
		    m_Target(target),
		    m_Up(up)
	{
		UpdateProjection();
		UpdateView();
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
	
	void PerspectiveCamera::SetPosition(const glm::vec3 &position)
	{
		m_Position = position;
		UpdateView();
	}
	
	void PerspectiveCamera::SetTarget(const glm::vec3 &target)
	{
		m_Target = target;
		UpdateView();
	}
	
	void PerspectiveCamera::SetUp(const glm::vec3 &up)
	{
		m_Up = up;
		UpdateView();
	}
	
	void PerspectiveCamera::UpdateProjection()
	{
		SetProjection(glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane));
	}
	
	void PerspectiveCamera::UpdateView()
	{
		SetView(glm::lookAt(m_Position, m_Target, m_Up));
	}
}
