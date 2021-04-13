#version 450 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

out VS_OUT 
{
    vec3 FragPosition;
    vec2 TexCoords;
} vs_out;

void main()
{
    vec3 fragPos = vec3(u_Transform * vec4(in_Position, 1.f));

	gl_Position = u_ProjectionViewMatrix * vec4(fragPos, 1.f);

    vs_out.FragPosition = fragPos;
    vs_out.TexCoords = in_TexCoords;
}