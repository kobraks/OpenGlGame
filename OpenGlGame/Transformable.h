#pragma once

namespace Game
{
	class Transformable
	{
		glm::vec3 m_Position = glm::vec3(0.f);
		glm::vec3 m_Rotation = glm::vec3(0.f);
		glm::vec3 m_Scale    = glm::vec3(1.f);

		mutable glm::mat4 m_Transform        = glm::mat4(1.f);
		mutable glm::mat4 m_InverseTransform = glm::mat4(1.f);

		mutable bool m_TransformNeedUpdate        = true;
		mutable bool m_InverseTransformNeedUpdate = true;
	public:
		Transformable() = default;
		explicit Transformable(const glm::vec3 &pos, const glm::vec3 &scale = glm::vec3(1.f), const glm::vec3 &rotate = glm::vec3(0.f));

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetRotation() const { return m_Rotation; }
		glm::vec3 GetScale() const { return m_Scale; }

		void SetPosition(const glm::vec3 &position);
		void Move(const glm::vec3 &vector);

		void SetRotation(const glm::vec3 &rotation);
		void Rotate(const glm::vec3 &vector);

		void SetScale(const glm::vec3 &scale);
		void Scale(const glm::vec3 &vector);

		const glm::mat4& GetTransform() const;
		const glm::mat4& GetInverseTransform() const;
	};
}
