#pragma once
#include "Engine/Core/Base.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Engine {
	class BufferLayout;

	struct Vertex {
		glm::vec3 Position = { 0.f, 0.f, 0.f };
		glm::vec2 TexCoords = { 0.f, 0.f };
		glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
		glm::vec3 Normal = { 0.f, 1.f, 0.f };

		static BufferLayout GetLayout();
	};
}
