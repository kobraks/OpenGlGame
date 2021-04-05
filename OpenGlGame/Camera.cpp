#include "pch.h"
#include "Camera.h"


namespace Game
{
	const glm::mat4& Camera::GetViewProjection() const
	{
		if(m_ViewProjectionNeedUpdate)
		{
			m_ViewProjectionNeedUpdate = false;
			m_ViewProjection           = m_Projection * m_View;
		}

		return m_ViewProjection;
	}
}
