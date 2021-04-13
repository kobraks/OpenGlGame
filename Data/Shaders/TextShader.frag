#version 330 core

uniform vec4 u_Color;
uniform sampler2D u_FontSampler;

in VS_OUT
{
	vec4 Color;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(u_FontSampler, fs_in.TexCoords).rgb, 1.f) * u_Color * fs_in.Color;
} 