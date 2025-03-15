#pragma once
#include "Engine/Scene/Components/CameraComponent.h"
#include "Engine/Scene/Components/IDComponent.h"
#include "Engine/Scene/Components/TagComponent.h"
#include "Engine/Scene/Components/TransformComponent.h"
#include "Engine/Scene/Components/FlagsComponent.h"

namespace Engine {
	template <typename... Component>
	struct ComponentGroup{};

	using DefaultComponents = ComponentGroup<IDComponent, TagComponent, FlagsComponent>;
	using AllComponents = ComponentGroup<IDComponent, TagComponent, TransformComponent, CameraComponent, FlagsComponent>;
}
