#include "pch.h"
#include "AssetManager.h"

#include "Image.h"

#include "Texture.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Material.h"


static constexpr std::string_view defaultVertexSource = R"(#version 450 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec3 in_Normal;
out VS_OUT{
	vec4 Color;
	vec3 Normal;
	vec2 TexCoords;
	vec3 FragPosition;
} vs_out;

//out int o_IsNormal;

void main()
{
	vec3 fragPos = vec3(u_Transform * vec4(in_Position, 1.f));

	//o_IsNormal = 0;

	vs_out.Color = in_Color;
	vs_out.Normal = mat3(transpose(u_InvertedTransform)) * in_Normal;
	vs_out.TexCoords = in_TexCoords;
	vs_out.FragPosition = fragPos;

	gl_Position = u_ProjectionViewMatrix * vec4(fragPos, 1.f);
})";

static constexpr std::string_view defaultFragmentSource = R"(#version 450 core
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

struct Light //TODO -Reorder stuff for better memory usage
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

	bool HasLightCookie;
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
uniform vec2 u_ViewPort;

uniform sampler2D u_LightCookie;

in VS_OUT{
	vec4 Color;
	vec3 Normal;
	vec2 TexCoords;
	vec3 FragPosition;
} fs_in;

out vec4 FragColor;


void main()
{
	if (u_Material.HasDiffuseTex)
		FragColor = vec4(vec3(texture(u_Material.DiffuseTex, texCoords)),1.f);
	else
		FragColor = vec4(u_Material.Diffuse, 1.f);
}
)";

namespace Game
{
	AssetManager::AssetManager()
	{
		m_DefaultMaterial = MakePointer<Material>();
		m_DefaultTexture = MakePointer<Texture>();

		const Image image(5, 5, Color(255, 1, 126));

		m_DefaultTexture->Create(image);
		m_DefaultTexture->SetWrapping(Wrapping::Repeat, Wrapping::Repeat);

		m_DefaultMaterial->DiffuseTexture = m_DefaultTexture;
		m_DefaultShader = MakePointer<ShaderProgram>();

		const auto vertexSource = ShaderSource(std::string(defaultVertexSource));
		const auto fragmentSource = ShaderSource(std::string(defaultFragmentSource));

		m_DefaultVertexShader = MakePointer<VertexShader>(vertexSource);
		m_DefaultFragmentShader = MakePointer<FragmentShader>(fragmentSource);

		m_DefaultShader->Attach(m_DefaultVertexShader);
		m_DefaultShader->Attach(m_DefaultFragmentShader);

		m_DefaultShader->Link();
	}

	bool AssetManager::LoadShader(const std::filesystem::path& path)
	{
		return false;
	}

	bool AssetManager::LoadTexture(const std::filesystem::path& path)
	{
		return false;
	}

	bool AssetManager::LoadModel(const std::filesystem::path& path)
	{
		return false;
	}

	void AssetManager::Update()
	{
		std::lock_guard guard(m_Mutex);

		while(!m_GlQueue.empty())
		{
			m_GlQueue.front()();
			m_GlQueue.pop();
		}

		for (auto order : m_Orders)
		{
		}

	}

	void AssetManager::Run()
	{
		std::lock_guard guard(m_Mutex);

		for (const auto order : m_Orders)
			order->Update();
	}
}
