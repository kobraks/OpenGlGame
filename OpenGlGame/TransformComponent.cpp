#include "pch.h"
#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Game
{
	TransformComponent::TransformComponent() : Component("Transform Component") {}

	TransformComponent::TransformComponent(Entity *entity) : Component("Transform Component", entity) {}

	void TransformComponent::SetPosition(const glm::vec3 &position)
	{
		m_Position = position;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}

	void TransformComponent::Move(const glm::vec3 &vector)
	{
		m_Position.x += vector.x;
		m_Position.y += vector.y;
		m_Position.z += vector.z;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}

	void TransformComponent::SetRotation(const glm::vec3 &rotation)
	{
		m_Rotation = rotation;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}

	void TransformComponent::Rotate(const glm::vec3 &vector)
	{
		m_Rotation.x += vector.x;
		m_Rotation.y += vector.y;
		m_Rotation.z += vector.z;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}

	void TransformComponent::SetScale(const glm::vec3 &scale)
	{
		m_Scale = scale;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}
	
	void TransformComponent::Scale(const glm::vec3 &vector)
	{
		m_Scale.x *= vector.x;
		m_Scale.y *= vector.y;
		m_Scale.z *= vector.z;

		m_TransformNeedUpdate        = true;
		m_InverseTransformNeedUpdate = true;
	}

	const glm::mat4& TransformComponent::GetTransform() const
	{
		if(m_TransformNeedUpdate)
		{
			m_TransformNeedUpdate        = false;
			m_InverseTransformNeedUpdate = true;

			m_Transform = glm::mat4(0.f);

			m_Transform = translate(m_Transform, m_Position);
			m_Transform = scale(m_Transform, m_Scale);

			m_Transform = rotate(m_Transform, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f));
			m_Transform = rotate(m_Transform, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f));
			m_Transform = rotate(m_Transform, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f));
		}

		return m_Transform;
	}

	const glm::mat4& TransformComponent::GetInverseTransform() const
	{
		if(m_InverseTransformNeedUpdate)
		{
			const auto &transform        = GetTransform();
			m_InverseTransformNeedUpdate = false;

			m_InverseTransform = inverse(transform);
		}

		return m_InverseTransform;
	}

	Pointer<Component> TransformComponent::Clone() const
	{
		return MakePointer<TransformComponent>(*this);
	}
}
