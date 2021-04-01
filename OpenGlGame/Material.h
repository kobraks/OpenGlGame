#pragma once
#include "Texture.h"

namespace Game
{
	class Mesh;
	class Model;
	
	class Material
	{		
	public:
		std::vector<Pointer<Texture>> Diffuse;
		std::vector<Pointer<Texture>> Specular;
		std::vector<Pointer<Texture>> Height;
		std::vector<Pointer<Texture>> Ambient;
		
		Material() = default;
	private:
		static Pointer<Material> ProcessMaterial(const void *aiMaterial);
		static std::vector<Pointer<Texture>> ProcessTexture(const void* aiMaterial, int textureType);

		friend Mesh;
		friend Model;
	};
}