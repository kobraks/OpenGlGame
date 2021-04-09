#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


namespace Game
{
	class BufferLayout;

	struct Vertex
	{
	public:
		Vertex() = default;

		Vertex(
			glm::vec3 position,
			glm::vec2 texCoords = glm::vec2(0.f, 0.f),
			glm::vec4 color     = glm::vec4(1.f, 1.f, 1.f, 1.f),
			glm::vec3 normal    = glm::vec3(0.f, 1.f, 0.f)
			) : Position(position),
			    TexCoords(texCoords),
			    Color(color),
			    Normal(normal) {}


		glm::vec3 Position = glm::vec3(0.f);
		glm::vec2 TexCoords = glm::vec2(0.f, 0.f);
		glm::vec4 Color = glm::vec4(1.f);
		glm::vec3 Normal = glm::vec3(0.f, 1.f, 0.f);

		static BufferLayout GetLayout();
	};
}
