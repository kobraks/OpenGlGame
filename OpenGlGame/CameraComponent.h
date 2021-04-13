#pragma once
#include "SceneCamera.h"
#include "Component.h"

namespace Game
{
	class CameraComponent : public Component
	{
		SceneCamera m_Camera;
		bool m_Primary = true;
		bool m_FixedAspectRatio = false;
	public:

		CameraComponent(Entity* entity = nullptr);

		bool IsPrimary() const { return m_Primary; }
		bool HasFixedAspectRatio() const { return m_FixedAspectRatio; }

		void IsPrimary(bool enable) { m_Primary = enable; }
		void HasFixedAspectRatio(bool enable) { m_FixedAspectRatio = enable; }

		const SceneCamera& GetCamera() const { return m_Camera; }
		SceneCamera& GetCamera() { return m_Camera; }
	};
}