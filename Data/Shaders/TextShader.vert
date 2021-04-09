#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

out vec4 Color;
out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPosition;

void main()
{
	FragPosition = vec3(u_Transform * vec4(in_Position, 1.f));

	Color = in_Color;
	TexCoords = in_TexCoords;

	gl_Position = u_ProjectionViewMatrix * vec4(FragPosition, 1.f);
}