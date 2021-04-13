#version 450 core

layout (location = 0) in vec3 in_Position;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_ModelInverse;

out VS_OUT 
{
    vec3 Normal;
    vec3 FragPosition;
} vs_out;

void main()
{
    vec3 fragPos = vec3(u_Model * vec4(in_Position, 1.f));

    gl_Position = u_View * vec4(fragPos, 1.f);
    mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));

    vs_out.FragPosition = fragPos;
    vs_out.Normal = normalize(vec3(vec4(normalMatrix * in_Normal, 0.f)));
}