#version 450 core

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

	float CutOff;
	float OuterCutOff;

	float Constant;
	float Linear;
	float Quadratic;
};

uniform Material u_Material;
uniform Light[MAX_LIGHTS] u_Light;

uniform bool u_CalcLights;
uniform vec3 u_CameraPos;
uniform bool u_WhiteColor;
uniform bool u_OnlyDiffuseTexture;

in vec4 o_Color;
in vec3 o_Normal;
in vec2 o_TexCoords;
in vec3 o_FragPosition;

out vec4 FragColor;

vec3 GetAmbient(Material material, Light light)
{
	vec3 ambient = vec3(0.f);

	if (material.HasDiffuseTex)
		ambient = light.Ambient * vec3(texture(material.DiffuseTex, o_TexCoords)) * material.Diffuse;
	else
		ambient = light.Ambient * material.Diffuse;

	return ambient;
}

vec3 GetDiffuse(Material material, Light light, float diff)
{
	vec3 diffuse = vec3(0.f);

	if (material.HasDiffuseTex)
		diffuse = light.Diffuse * diff * vec3(texture(material.DiffuseTex, o_TexCoords)) * material.Diffuse;
	else
		diffuse = light.Diffuse * diff * material.Diffuse;

	return diffuse;
}

vec3 GetSpecular(Material material, Light light, float spec)
{
	vec3 specular = vec3(0.f);

	if (material.HasSpecularTex)
		specular = light.Specular * spec * vec3(texture(material.SpecularTex, o_TexCoords)) * material.Diffuse;
	else
		specular = light.Specular * spec * material.Specular;

	return specular;
}

vec4 CalcDirectionalLight(Light light, Material material, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);
	float diff = max(dot(normal, lightDir), 0.f);

	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.Shininess);

	vec3 ambient = GetAmbient(material, light);
	vec3 diffuse = GetDiffuse(material, light, diff);
	vec3 specular = GetSpecular(material, light, spec);
	
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

	vec3 ambient = GetAmbient(material, light);
	vec3 diffuse = GetDiffuse(material, light, diff);
	vec3 specular = GetSpecular(material, light, spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4((ambient + diffuse + specular), 1.f);
}

vec4 CalcSpotLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - fragPos);

	float diff = max(dot(normal, lightDir), 0.f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.Shininess);

	float dist = length(light.Position - fragPos);
	float attenuation = 1.f / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.CutOff - light.OuterCutOff;
	float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.f, 1.f);

	vec3 ambient = GetAmbient(material, light);
	vec3 diffuse = GetDiffuse(material, light, diff);
	vec3 specular = GetSpecular(material, light, spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4((ambient + diffuse + specular), 1.f);
}

void main()
{
	vec4 result = vec4(0.f);

	if (u_CalcLights)
	{
		vec3 viewDir = normalize(u_CameraPos - o_FragPosition);

		for (uint i = 0u; i < MAX_LIGHTS; ++i)
		{
			if (u_Light[i].Active)
			{
				if (u_Light[i].Type == DIRECTIONAL_LIGHT)
					result += CalcDirectionalLight(u_Light[i], u_Material, o_Normal, viewDir);
				else if (u_Light[i].Type == POINT_LIGHT)
					result += CalcPointLight(u_Light[i], u_Material, o_Normal, o_FragPosition, viewDir);
				else if (u_Light[i].Type == SPOT_LIGHT)
					result += CalcSpotLight(u_Light[i], u_Material, o_Normal, o_FragPosition, viewDir);
			}
		}
	}
	else
	{
		if (u_Material.HasDiffuseTex)
			result = texture(u_Material.DiffuseTex, o_TexCoords) * o_Color;
		else
			result = vec4(u_Material.Diffuse, 1.f) * o_Color;
	}

	if (u_OnlyDiffuseTexture)
		FragColor = vec4(texture(u_Material.DiffuseTex, o_TexCoords).rgb, 1.f);
	else if (!u_WhiteColor)
		FragColor = result;

	if (u_WhiteColor)
		FragColor = vec4(1.f);
	//FragColor = texture(u_Material.DiffuseTex, o_TexCoords) * vec4(u_Material.Diffuse, 1.f) * Color;
} 