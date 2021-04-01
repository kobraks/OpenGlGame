#pragma once

#include <glm/glm.hpp>

namespace Game
{
	class Camera
	{
		glm::mat4 m_Projection = glm::mat4(1.f);
		glm::mat4 m_View = glm::mat4(1.f);

		mutable bool m_ViewProjectionNeedUpdate = true;
		mutable glm::mat4 m_ViewProjection;
	public:
		Camera() = default;
		Camera(const glm::mat4 &projection) : m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewProjection() const
		{
			if (m_ViewProjectionNeedUpdate)
			{
				m_ViewProjectionNeedUpdate = false;
				m_ViewProjection = m_View * m_Projection;
			}

			return m_ViewProjection;
		}
		const glm::mat4& GetView() const { return m_View; }

	protected:
		void SetProjection(const glm::mat4& projection) { m_Projection = projection; m_ViewProjectionNeedUpdate = true; }
		void SetView(const glm::mat4& view) { m_View = view; m_ViewProjectionNeedUpdate = true; }
	
	};
}
