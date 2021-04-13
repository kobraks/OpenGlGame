#version 450 core

#define SPOT_LIGHT 1
#define POINT_LIGHT 2
#define DIRECTIONAL_LIGHT 3
#define MAX_LIGHTS 10

struct Material
{
	//vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	float Shininess;
	float ShininessStrength;

	bool HasDiffuseTex;
	bool HasSpecularTex;

	sampler2D DiffuseTex;
	sampler2D SpecularTex;
	//sampler2D NormalMap;
};

struct Light
{
	bool Active;
	int Type;

	vec3 Position;
	vec3 Direction;

	vec3 Diffuse;
	vec3 Ambient;
	vec3 Specular;

	float CutOff;
	float OuterCutOff;

	float Constant;
	float Linear;
	float Quadratic;
};

layout(std140) uniform u_Lights
{
	Light[MAX_LIGHTS] Lights;
};

uniform Material u_Material;

uniform bool u_CalcLights;
uniform vec3 u_CameraPos;
uniform bool u_WhiteColor;
uniform bool u_OnlyDiffuseTexture;
uniform float u_GammaCorrection;

in VS_OUT{
	vec4 Color;
	vec3 Normal;
	vec2 TexCoords;
	vec3 FragPosition;
} fs_in;

out vec4 FragColor;

vec4 CalcDirectionalLight(Light light, Material material, vec3 normal, vec3 viewDir, vec2 texCoords);
vec4 CalcPointLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
vec4 CalcSpotLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);

void main()
{
	vec4 result = vec4(0.f);

	if (u_CalcLights)
	{
		vec3 viewDir = normalize(u_CameraPos - fs_in.FragPosition);
		vec3 normal = normalize(fs_in.Normal);

		for (int i = 0; i < MAX_LIGHTS; ++i)
		{
			Light light = Lights[i];

			if (light.Active)
			{
				if (light.Type == DIRECTIONAL_LIGHT)
					result += CalcDirectionalLight(light, u_Material, normal, viewDir, fs_in.TexCoords);
				else if (light.Type == POINT_LIGHT)
					result += CalcPointLight(light, u_Material, normal, fs_in.FragPosition, viewDir, fs_in.TexCoords);
				else if (light.Type == SPOT_LIGHT)
					result += CalcSpotLight(light, u_Material, normal, fs_in.FragPosition, viewDir, fs_in.TexCoords);
			}
		}
	}
	else
	{
		if (u_Material.HasDiffuseTex)
			result = texture(u_Material.DiffuseTex, fs_in.TexCoords) * fs_in.Color;
		else
			result = vec4(u_Material.Diffuse, 1.f) * fs_in.Color;
	}

	result.rgb = pow(result.rgb, vec3(1.f / u_GammaCorrection));

	if (u_OnlyDiffuseTexture)
		FragColor = vec4(texture(u_Material.DiffuseTex, fs_in.TexCoords).rgb, 1.f);
	else if (!u_WhiteColor)
		FragColor = result;

	if (u_WhiteColor)
		FragColor = vec4(1.f);

	//FragColor = texture(u_Material.DiffuseTex, o_TexCoords) * vec4(u_Material.Diffuse, 1.f) * Color;
} 

vec3 GetAmbient(Material material, Light light, vec2 texCoords)
{
	vec3 ambient = vec3(0.f);

	if (material.HasDiffuseTex)
		ambient = light.Ambient * vec3(texture(material.DiffuseTex, texCoords)) * material.Diffuse;
	else
		ambient = light.Ambient * material.Diffuse;

	return ambient;
}

vec3 GetDiffuse(Material material, Light light, float diff, vec2 texCoords)
{
	vec3 diffuse = vec3(0.f);

	if (material.HasDiffuseTex)
		diffuse = light.Diffuse * diff * vec3(texture(material.DiffuseTex, texCoords)) * material.Diffuse;
	else
		diffuse = light.Diffuse * diff * material.Diffuse;

	return diffuse;
}

vec3 GetSpecular(Material material, Light light, float spec, vec2 texCoords)
{
	vec3 specular = vec3(0.f);

	if (material.HasSpecularTex)
		specular = light.Specular * spec * vec3(texture(material.SpecularTex, texCoords)) * material.Specular;
	else
		specular = light.Specular * spec * material.Specular;

	return specular;
}

vec4 CalcDirectionalLight(Light light, Material material, vec3 normal, vec3 viewDir, vec2 texCoords)
{
	vec3 lightDir = normalize(-light.Direction);
	float diff = max(dot(normal, lightDir), 0.f);


	vec3 halfwayDir = normalize(lightDir + viewDir);
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.Shininess);

	float spec = pow(max(dot(normal, halfwayDir), 0.f), material.Shininess / material.ShininessStrength);

	vec3 ambient = GetAmbient(material, light, texCoords);
	vec3 diffuse = GetDiffuse(material, light, diff, texCoords);
	vec3 specular = GetSpecular(material, light, spec, texCoords);
	
	return vec4((ambient + diffuse + specular), 1.f);
}

vec4 CalcPointLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
	vec3 lightDir = normalize(light.Position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.f), material.Shininess / material.ShininessStrength);

	float dist = length(light.Position - fragPos);
	float attenuation = 1.f / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

	vec3 ambient = GetAmbient(material, light, texCoords);
	vec3 diffuse = GetDiffuse(material, light, diff, texCoords);
	vec3 specular = GetSpecular(material, light, spec, texCoords);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4((ambient + diffuse + specular), 1.f);
}

vec4 CalcSpotLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
	vec3 lightDir = normalize(light.Position - fragPos);

	float diff = max(dot(normal, lightDir), 0.f);

	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.Shininess);

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.f), material.Shininess / material.ShininessStrength);

	float dist = length(light.Position - fragPos);
	float attenuation = 1.f / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

	float theta = dot(lightDir, normalize(-light.Direction));
	float epsilon = light.CutOff - light.OuterCutOff;
	float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.f, 1.f);

	vec3 ambient = GetAmbient(material, light, texCoords);
	vec3 diffuse = GetDiffuse(material, light, diff, texCoords);
	vec3 specular = GetSpecular(material, light, spec, texCoords);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4((ambient + diffuse + specular), 1.f);
}