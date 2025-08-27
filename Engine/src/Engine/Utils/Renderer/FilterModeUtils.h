#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	inline constexpr FilterMode SanitizeMag(FilterMode mode) {
		return (mode == FilterMode::Nearest || mode == FilterMode::Linear) ? mode : FilterMode::Linear;
	}
}