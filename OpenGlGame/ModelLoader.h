#pragma once
#include <vector>

#include "Types.h"

namespace Game
{
	class Model;
	class Mesh;
	struct Material;
	class Texture;

	class ModelLoader
	{
	public:
		static Pointer<Model> Load(const std::string &fileName, const std::string &directory);
	public:
		static void ProcessNode(std::vector<Pointer<Mesh>> &meshes, const glm::mat4 &transform, const void *aiNode, const void *aiScene);
		
		static Pointer<Mesh> ProcessMesh(const void *aiMesh, const void *aiScene);
		static Pointer<Material> ProcessMaterial(const void *aiMaterial);

		static std::vector<Pointer<Texture>> ProcessTextures(const void *aiMaterial, int textureType);
		static glm::vec3 ProcessColor(const void *aiMaterial, int colorType);
	};
}
