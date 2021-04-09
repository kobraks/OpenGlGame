#version 330 core

uniform vec4 u_Color;
uniform sampler2D u_FontSampler

in vec4 Color;
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPosition;

out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(u_FontSampler, TexCoords).rgb, 1.f) * u_Color * Color;
} 