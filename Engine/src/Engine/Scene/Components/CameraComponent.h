#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Scene/SceneCamera.h"

namespace Engine {
	struct CameraComponent {
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(CameraComponent&) = default;
	};
}