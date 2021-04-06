#include "pch.h"
#include "Material.h"

#include "Texture.h"
#include "TextureLoader.h"
#include "ShaderProgram.h"

#include <assimp/scene.h>

namespace Game
{
	constexpr static std::string_view ToString(aiTextureType textureType)
	{
		switch(textureType)
		{
			case aiTextureType_DIFFUSE: return "Diffuse Texture";

			case aiTextureType_SPECULAR: return "Specular Texture";

			default: return "Unknown";
		}
	}

	constexpr static std::string_view ToString(Material::ColorType textureType)
	{
		switch(textureType)
		{
			case Material::ColorType::Diffuse: return "Diffuse Color";

			case Material::ColorType::Specular: return "Specular Color";

			default: return "Unknown";
		}
	}


	Pointer<Material> Material::ProcessMaterial(const void *aiMaterial)
	{
		auto material = MakePointer<Material>();

		const auto mat = static_cast<const ::aiMaterial*>(aiMaterial);
		material->Name = mat->GetName().C_Str();

		LOG_DEBUG("Processing material: {}", material->Name);

		ProcessTextures(material, aiMaterial);
		ProcessColors(material, aiMaterial);

		return material;
	}

	void Material::ProcessTextures(Pointer<Material> &material, const void *aiMaterial)
	{
		if(const auto textures = ProcessTexture(aiMaterial, aiTextureType_DIFFUSE); !textures.empty())
			material->DiffuseTexture = textures[0];

		if(const auto textures = ProcessTexture(aiMaterial, aiTextureType_SPECULAR); !textures.empty())
			material->SpecularTexture = textures[0];
	}

	void Material::ProcessColors(Pointer<Material> &material, const void *aiMaterial)
	{
		material->Shininess = ProcessShininess(aiMaterial);

		material->DiffuseColor  = ProcessColor(aiMaterial, ColorType::Diffuse);
		material->SpecularColor = ProcessColor(aiMaterial, ColorType::Specular);
	}

	std::vector<Pointer<Texture>> Material::ProcessTexture(const void *aiMaterial, int textureType)
	{
		LOG_DEBUG("Processing {}", ToString(static_cast<aiTextureType>(textureType)));
		const auto mat = static_cast<const ::aiMaterial*>(aiMaterial);

		const size_t textureCount = mat->GetTextureCount(static_cast<aiTextureType>(textureType));
		LOG_DEBUG("{} amount: {}", ToString(static_cast<aiTextureType>(textureType)), textureCount);

		std::vector<Pointer<Texture>> textures;
		textures.reserve(textureCount);

		for(size_t i = 0; i < textureCount; ++i)
		{
			aiString str;
			mat->GetTexture(static_cast<aiTextureType>(textureType), i, &str);

			textures.push_back(TextureLoader::Load(str.C_Str(), "../Data/Textures"));
		}

		return textures;
	}

	glm::vec3 Material::ProcessColor(const void *aiMaterial, ColorType colorType)
	{
		const auto translate = [](const aiColor3D &color) { return glm::vec3(color.r, color.g, color.b); };

		const auto mat = static_cast<const ::aiMaterial*>(aiMaterial);

		aiColor3D color;
		switch(colorType)
		{
			case ColorType::Diffuse: mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				break;
			case ColorType::Specular: mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
				break;
			case ColorType::Ambient: mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
				break;
		}

		const auto result = translate(color);

		LOG_DEBUG("Processing {}, Color (R: {}, G: {}, B: {})", ToString(colorType), result.x, result.y, result.z);
		
		return result;
	}

	float Material::ProcessShininess(const void *aiMaterial)
	{
		const auto mat = static_cast<const ::aiMaterial*>(aiMaterial);

		float shininess = 0.f;

		mat->Get(AI_MATKEY_SHININESS, shininess);

		LOG_DEBUG("Shininess level: ", shininess);
		
		return shininess;
	}
}
