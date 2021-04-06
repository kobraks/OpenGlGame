#pragma once

namespace Game
{
	class Mesh;
	class Model;
	class Texture;

	class Material
	{
	public:
		enum class ColorType
		{
			Diffuse,
			Specular,
			Ambient
		};
		
		// glm::vec3 AmbientColor  = glm::vec3(1.f);
		glm::vec3 DiffuseColor  = glm::vec3(1.f);
		glm::vec3 SpecularColor = glm::vec3(1.f);

		float Shininess = 0.1f;

		Pointer<Texture> DiffuseTexture  = nullptr;
		Pointer<Texture> SpecularTexture = nullptr;

		std::string Name;

		Material() = default;
	private:
		static Pointer<Material> ProcessMaterial(const void *aiMaterial);
		static void ProcessTextures(Pointer<Material>& material, const void *aiMaterial);
		static void ProcessColors(Pointer<Material>&material, const void *aiMaterial);

		static std::vector<Pointer<Texture>> ProcessTexture(const void *aiMaterial, int textureType);

		static glm::vec3 ProcessColor(const void *aiMaterial, ColorType colorType);
		static float ProcessShininess(const void *aiMaterial);
		
		friend Mesh;
		friend Model;
	};
}
