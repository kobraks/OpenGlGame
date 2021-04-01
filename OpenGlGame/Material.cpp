#include "pch.h"
#include "Material.h"

#include "ShaderProgram.h"
#include "TextureLoader.h"

#include <assimp/scene.h>

namespace Game
{
	Pointer<Material> Material::ProcessMaterial(const void *aiMaterial)
	{
		auto material = MakePointer<Material>();

		material->Diffuse = ProcessTexture(aiMaterial, aiTextureType_DIFFUSE);
		material->Ambient = ProcessTexture(aiMaterial, aiTextureType_AMBIENT);
		material->Height = ProcessTexture(aiMaterial, aiTextureType_HEIGHT);
		material->Specular = ProcessTexture(aiMaterial, aiTextureType_SPECULAR);
		
		return material;
	}

	std::vector<Pointer<Texture>> Material::ProcessTexture(const void *aiMaterial, int textureType)
	{
		const auto mat = static_cast<const ::aiMaterial*>(aiMaterial);
		const size_t textureCount = mat->GetTextureCount(static_cast<aiTextureType>(textureType));
		
		std::vector<Pointer<Texture>> textures;
		textures.reserve(textureCount);

		for (size_t i = 0; i < textureCount; ++i)
		{
			aiString str;
			mat->GetTexture(static_cast<aiTextureType>(textureType), i, &str);
			
			textures.push_back(TextureLoader::Load(str.C_Str(), "Textures"));
		}

		return textures;
	}
}
