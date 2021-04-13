#pragma once

namespace Game
{
	class Mesh;
	class Model;
	class Texture;

	struct Material
	{
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
		float ShininessStrength = 1.f;

		Pointer<Texture> DiffuseTexture  = nullptr;
		Pointer<Texture> SpecularTexture = nullptr;

		std::string Name;
	};
}
