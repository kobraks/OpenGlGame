#version 330 core

#define SPOT_LIGHT 1
#define POINT_LIGHT 2
#define DIRECTIONAL_LIGHT 3
#define MAX_LIGHTS 10u

struct Material
{
	//vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	float Shininess;

	bool HasDiffuseTex;
	bool HasSpecularTex;

	sampler2D DiffuseTex;
	sampler2D SpecularTex;
	//sampler2D NormalMap;
};

struct Light
{
	vec3 Position;
	vec3 Direction;
	int Type;

	bool Active;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	float AngleInnerCone;
	float AngleOuterCone;

	float Constant;
	float Linear;
	float Quadratic;
};

uniform Material u_Material;
uniform Light[MAX_LIGHTS] u_Light;

uniform bool u_CalcLights;
uniform vec3 u_CameraPos;

in vec4 Color;
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPosition;

out vec4 FragColor;

vec4 CalcDirectionalLight(Light light, Material material, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);
	float diff = max(dot(normal, lightDir), 0.f);

	vec3 reflectDir = reflect(-light.Direction, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.Shininess);

	vec3 ambient = vec3(0.f);
	vec3 diffuse = vec3(0.f);
	vec3 specular = vec3(0.f);
	
	if (material.HasDiffuseTex)
	{
		ambient = light.Ambient * vec3(texture(material.DiffuseTex, TexCoords));
		diffuse = light.Diffuse * diff * vec3(texture(material.DiffuseTex, TexCoords));
	}
	else
	{
		ambient = light.Ambient * material.Diffuse;
		diffuse = light.Diffuse * diff * material.Diffuse;
	}

	if (material.HasSpecularTex)
		specular = light.Specular * spec * vec3(texture(material.SpecularTex, TexCoords));
	else
		specular = light.Specular * spec * material.Specular;

	return vec4((ambient + diffuse + specular), 1.f);
}

vec4 CalcPointLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

	float dist = length(light.Position - fragPos);
	float attenuation = 1.f / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

	vec3 ambient = vec3(0.f);
	vec3 diffuse = vec3(0.f);
	vec3 specular = vec3(0.f);
	
	if (material.HasDiffuseTex)
	{
		ambient = light.Ambient * vec3(texture(material.DiffuseTex, TexCoords));
		diffuse = light.Diffuse * diff * vec3(texture(material.DiffuseTex, TexCoords));
	}
	else
	{
		ambient = light.Ambient * material.Diffuse;
		diffuse = light.Diffuse * diff * material.Diffuse;
	}

	if (material.HasSpecularTex)
		specular = light.Specular * spec * vec3(texture(material.SpecularTex, TexCoords));
	else
		specular = light.Specular * spec * material.Specular;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4((ambient + diffuse + specular), 1.f);
}

vec4 CalcSpotLight(Light light, Material material)
{
	return vec4(1.f, 1.f, 1.f, 1.f);
}

void main()
{
	vec4 output = vec4(0.f);

	if (u_CalcLights)
	{
		vec3 viewDir = normalize(u_CameraPos - FragPos);

		for (uint i = 0u; i < MAX_LIGHTS; ++i)
		{
			if (u_Light[i].Active)
			{
				if (u_Light[i].Type == DIRECTIONAL_LIGHT)
					output += CalcDirectionalLight(u_Light[i], u_Material, Normal, viewDir);
				else if (u_Light[i].Type == POINT_LIGHT)
					output += CalcPointLight(u_Light[i], u_Material, Normal, FragPosition, viewDir);
				else if (u_Light[i].Type == SPOT_LIGHT)
					output += CalcSpotLight(u_Light[i], u_Material);
			}
		}
	}

	FragColor = output;
} 