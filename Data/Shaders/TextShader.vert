#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

out VS_OUT
{
	vec4 Color;
	vec2 TexCoords;
} vs_out;


void main()
{
	vec3 fragPos = vec3(u_Transform * vec4(in_Position, 1.f));

	vs_out.Color = in_Color;
	vs_out.TexCoords = in_TexCoords;

	gl_Position = u_ProjectionViewMatrix * vec4(fragPos, 1.f);
}