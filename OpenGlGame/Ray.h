#pragma once

#include <glm/vec3.hpp>

namespace Game
{
	class Ray
	{
		glm::vec3 m_Origin;
		glm::vec3 m_Direction;
	public:
		Ray(const glm::vec3& origin, const glm::vec3 direction) : m_Origin(origin), m_Direction(direction) {}

		glm::vec3 GetDirection() const {return m_Direction; }
		glm::vec3 GetOrigin() const { return m_Origin; }

		glm::vec3 operator()(float distance) const { return m_Origin + distance * m_Direction; }
	};
}