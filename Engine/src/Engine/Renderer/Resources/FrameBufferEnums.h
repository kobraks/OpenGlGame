#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	enum class Channel : uint8_t {
		Red = 0,
		Green,
		Blue,
		Alpha
	};

	enum class BlitFilter {
		Nearest = 0,
		Linear
	};

	enum class BlitMask : uint8_t {
		Color = BIT(0),
		Depth = BIT(1),
		Stencil = BIT(2),

		ColorAndDepth = BIT(0) | BIT(1),
		ColorAndStencil = BIT(0) | BIT(2),
		DepthAndStencil = BIT(1) | BIT(2),
	};
}