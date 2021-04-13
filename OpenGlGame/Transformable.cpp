#include "pch.h"
#include "Transformable.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Game::Transformable::Transformable(const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate) : m_Position(pos),
                                                                                                    m_Rotation(rotate),
                                                                                                    m_Scale(scale) {}

void Game::Transformable::SetPosition(const glm::vec3 &position)
{
	m_Position = position;

	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

void Game::Transformable::Move(const glm::vec3 &vector)
{
	m_Position += vector;

	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

void Game::Transformable::SetRotation(const glm::vec3 &rotation)
{
	m_Rotation = rotation;

	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

void Game::Transformable::Rotate(const glm::vec3 &vector)
{
	m_Rotation += vector;
	
	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

void Game::Transformable::SetScale(const glm::vec3 &scale)
{
	m_Scale = scale;

	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

void Game::Transformable::Scale(const glm::vec3 &vector)
{
	m_Scale *= vector;

	m_TransformNeedUpdate        = true;
	m_InverseTransformNeedUpdate = true;
}

const glm::mat4& Game::Transformable::GetTransform() const
{
	if(m_TransformNeedUpdate)
	{
		m_TransformNeedUpdate        = false;
		m_InverseTransformNeedUpdate = true;

		m_Transform = glm::mat4(1.f);

		m_Transform = translate(m_Transform, m_Position);
		m_Transform = m_Transform * glm::toMat4(glm::quat(m_Rotation));
		m_Transform = glm::scale(m_Transform, m_Scale);
		
		// m_Transform = rotate(m_Transform, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f));
		// m_Transform = rotate(m_Transform, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f));
		// m_Transform = rotate(m_Transform, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f));
	}

	return m_Transform;
}

const glm::mat4& Game::Transformable::GetInverseTransform() const
{
	if(m_InverseTransformNeedUpdate)
	{
		const auto &transform        = GetTransform();
		m_InverseTransformNeedUpdate = false;

		m_InverseTransform = inverse(transform);
	}

	return m_InverseTransform;
}
