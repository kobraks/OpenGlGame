#pragma once
#include "Engine/Core/Base.h"

#include <glm/mat4x4.hpp>

namespace Engine {
	class Camera {
	public:
		Camera() = default;
		explicit Camera(const glm::mat4 &projection) : m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4 &GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.f);
	};
}