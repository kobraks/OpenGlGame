#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


namespace Game
{
	class BufferLayout;
	
	class Vertex
	{
	public:
		Vertex() = default;

		Vertex(glm::vec3 position, glm::vec2 texCoords = glm::vec2(0), glm::vec4 color = glm::vec4(0), glm::vec3 normal = glm::vec3(0)) : Position(position),
			TexCoords(texCoords),
			Color(color),
			Normal(normal) {}


		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec4 Color;
		glm::vec3 Normal;

		static BufferLayout GetLayout();
	};
}
