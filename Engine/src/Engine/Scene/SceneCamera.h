#pragma once

#include "Engine/Renderer/Camera.h"

#include <glm/trigonometric.hpp>

namespace Game
{
	class SceneCamera: public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		float m_PerspectiveFOV  = glm::radians(45.f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar  = 1000.f;

		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.f;
		float m_OrthographicFar  = 1.f;

		float m_AspectRatio = 0.f;
	public:
		SceneCamera();
		~SceneCamera() override = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }

		void SetPerspectiveVerticalFOVRadians(float verticalFov)
		{
			m_PerspectiveFOV = verticalFov;
			RecalculateProjection();
		}

		void SetPerspectiveVerticalFOV(float verticalFov)
		{
			m_PerspectiveFOV = glm::radians(verticalFov);
			RecalculateProjection();
		}

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }

		void SetPerspectiveNearClip(float nearClip)
		{
			m_PerspectiveNear = nearClip;
			RecalculateProjection();
		}

		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		void SetPerspectiveFarClip(float farClip)
		{
			m_PerspectiveFar = farClip;
			RecalculateProjection();
		}

		float GetOrthographicSize() const { return m_OrthographicSize; }

		void SetOrthographicSize(float size)
		{
			m_OrthographicSize = size;
			RecalculateProjection();
		}

		float GetOrthographicNearClip() const { return m_OrthographicNear; }

		void SetOrthographicNearClip(float nearClip)
		{
			m_OrthographicNear = nearClip;
			RecalculateProjection();
		}

		float GetOrthographicFarClip() const { return m_OrthographicFar; }

		void SetOrthographicFarClip(float farClip)
		{
			m_OrthographicFar = farClip;
			RecalculateProjection();
		}

		ProjectionType GetProjectionType() const { return m_ProjectionType; }

		void SetProjectionType(ProjectionType type)
		{
			m_ProjectionType = type;
			RecalculateProjection();
		}

		float GetAspectRatio() const { return m_AspectRatio; }

		void SetAspectRatio(float aspectRatio)
		{
			m_AspectRatio = aspectRatio;
			RecalculateProjection();
		}

	private:
		void RecalculateProjection();
	};
}
