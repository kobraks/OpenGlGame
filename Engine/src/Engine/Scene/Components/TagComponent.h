#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(TagComponent&) = default;
		TagComponent(const std::string &tag) : Tag(tag) {}
	};
}