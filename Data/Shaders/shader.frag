#version 330 core

struct Material
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shinines;

	sampler2D DiffuseTex;
	sampler2D SpecularTex;
	sampler2D NormalMap;
};

struct Light
{
	vec3 Position;
	vec3 Direction;
	int Type;
};

uniform vec4 u_Color;

uniform Material u_Material;
uniform Light u_Light;

in vec4 Color;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = u_Color * Color;
} 