#include "pch.h"
#include "ModelLoader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "TextureLoader.h"

#define ASSIMP_FLAGS\
	aiProcess_JoinIdenticalVertices |\
	aiProcess_Triangulate |\
	aiProcess_GenNormals |\
	aiProcess_GenUVCoords |\
	aiProcess_OptimizeMeshes |\
	aiProcess_FixInfacingNormals

constexpr int COLOR_DIFFUSE  = 0;
constexpr int COLOR_SPECULAR = 1;
constexpr int COLOR_AMBIENT  = 2;

namespace Game
{
	constexpr static std::string_view ToStringColor(int colorType)
	{
		switch(colorType)
		{
			case COLOR_DIFFUSE: return "Diffuse Color";
			case COLOR_SPECULAR: return "Specular Color";
			case COLOR_AMBIENT: return "Ambient Color";

			default: return "Unknown";
		}
	}

	constexpr static std::string_view ToString(aiTextureType textureType)
	{
		switch(textureType)
		{
			case aiTextureType_DIFFUSE: return "Diffuse Texture";

			case aiTextureType_SPECULAR: return "Specular Texture";

			default: return "Unknown";
		}
	}

	static inline glm::mat4 Convert(const aiMatrix4x4 &m)
	{
		glm::mat4 mat(1.0f);
		mat[0] = glm::vec4(m.a1, m.b1, m.c1, m.d1);
		mat[1] = glm::vec4(m.a2, m.b2, m.c2, m.d2);
		mat[2] = glm::vec4(m.a3, m.b3, m.c3, m.d3);
		mat[3] = glm::vec4(m.a4, m.b4, m.c4, m.d4);
		
		return mat;
	}

	static inline glm::vec2 Convert(const aiVector2D &vector)
	{
		return glm::vec2(vector.x, vector.y);
	}

	static inline glm::vec3 Convert(const aiVector3D &vector)
	{
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	static inline glm::vec4 Convert(const aiColor4D &vector)
	{
		return glm::vec4(vector.r, vector.g, vector.b, vector.a);
	}

	static inline glm::vec3 Convert(const aiColor3D &color)
	{
		return glm::vec3(color.r, color.g, color.b);
	}

	Pointer<Model> ModelLoader::Load(const std::string &fileName, const std::string &directory)
	{
		const std::string name = directory + "/" + fileName;

		LOG_INFO("Opening \"{}\"", name);

		auto importer     = MakePointer<Assimp::Importer>();
		const auto *scene = importer->ReadFile(name, !aiProcess_JoinIdenticalVertices & aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_PreTransformVertices | aiProcess_GenNormals);

		if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("Unable to import \"{}\", {}", name, importer->GetErrorString());
			return nullptr;
		}

		LOG_INFO("Scene imported sucessfuly");
		const auto meshesCount = scene->mNumMeshes;
		LOG_DEBUG("Scene meshes count: {}", meshesCount);

		Model::MeshContainerType meshes;
		meshes.reserve(meshesCount);

		ProcessNode(meshes, glm::mat4(1.f), scene->mRootNode, scene);

		Pointer<Model> model = MakePointer<Model>(meshes);
		return model;
	}

	void ModelLoader::ProcessNode(std::vector<Pointer<Mesh>> &meshes, const glm::mat4 &transform, const void *aiNode, const void *aiScene)
	{
		const auto node  = static_cast<const ::aiNode*>(aiNode);
		const auto scene = static_cast<const ::aiScene*>(aiScene);

		for(uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			const auto mesh = scene->mMeshes[node->mMeshes[i]];
			auto m          = ProcessMesh(mesh, scene);
			m->SetMaterial(ProcessMaterial(scene->mMaterials[mesh->mMaterialIndex]));
			m->SetTransform(transform);
			meshes.push_back(m);
		}

		const glm::mat4 nodeTransform = Convert(node->mTransformation);

		for(uint32_t i = 0; i < node->mNumChildren; ++i)
			ProcessNode(meshes, transform * nodeTransform, node->mChildren[i], scene);
	}

	Pointer<Mesh> ModelLoader::ProcessMesh(const void *aiMesh, const void *aiScene)
	{
		const auto scene = static_cast<const ::aiScene*>(aiScene);
		const auto mesh  = static_cast<const ::aiMesh*>(aiMesh);

		const std::string name = mesh->mName.C_Str();

		const uint32_t verticesCount = mesh->mNumVertices;
		const uint32_t facesCount    = mesh->mNumFaces;
		const uint32_t indicesCount  = facesCount * 3u;

		LOG_INFO("Processing Mesh: {}", name);
		LOG_DEBUG("Vertices count: {}", verticesCount);
		LOG_DEBUG("Faces count: {}", verticesCount);
		LOG_DEBUG("Indices count: {}", indicesCount);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve(verticesCount);
		indices.reserve(indicesCount);

		if(!mesh->HasNormals())
			LOG_WARN("Mesh has not normal vectors");

		LOG_DEBUG("Processing vertices");
		for(size_t i = 0; i < verticesCount; ++i)
		{
			Vertex vertex{};

			vertex.Position = Convert(mesh->mVertices[i]);

			if(mesh->HasVertexColors(0))
				vertex.Color = Convert(mesh->mColors[0][i]);

			if(mesh->HasTextureCoords(0))
				vertex.TexCoords = Convert(mesh->mTextureCoords[0][i]);

			if(mesh->HasNormals())
				vertex.Normal = Convert(mesh->mNormals[i]);

			vertices.emplace_back(vertex);
		}

		LOG_DEBUG("Processing faces");
		if(mesh->HasFaces())
		{
			for(size_t i = 0; i < mesh->mNumFaces; ++i)
			{
				const auto &face = mesh->mFaces[i];

				for(size_t j = 0; j < face.mNumIndices; ++j)
					indices.emplace_back(face.mIndices[j]);
			}
		}

		return MakePointer<Mesh>(name, vertices, indices);
	}

	Pointer<Material> ModelLoader::ProcessMaterial(const void *aiMaterial)
	{
		const auto material = static_cast<const ::aiMaterial*>(aiMaterial);

		auto result  = MakePointer<Material>();
		result->Name = material->GetName().C_Str();

		LOG_INFO("Processing Material: {}", result->Name);

		if(const auto textures = ProcessTextures(material, aiTextureType_DIFFUSE); !textures.empty())
			result->DiffuseTexture = textures[0];

		if(const auto textures = ProcessTextures(material, aiTextureType_SPECULAR); !textures.empty())
			result->SpecularTexture = textures[0];

		result->DiffuseColor  = ProcessColor(material, COLOR_DIFFUSE);
		result->SpecularColor = ProcessColor(material, COLOR_SPECULAR);

		material->Get(AI_MATKEY_SHININESS, result->Shininess);
		LOG_DEBUG("Shininess level: {}", result->Shininess);

		return result;
	}

	std::vector<Pointer<Texture>> ModelLoader::ProcessTextures(const void *aiMaterial, int textureType)
	{
		const auto type             = static_cast<aiTextureType>(textureType);
		const auto material         = static_cast<const ::aiMaterial*>(aiMaterial);
		const uint32_t textureCount = material->GetTextureCount(type);

		LOG_DEBUG("Processing {}, textures count: {}", ToString(type), textureCount);

		std::vector<Pointer<Texture>> textures;
		textures.reserve(textureCount);

		for(uint32_t i = 0; i < textureCount; ++i)
		{
			aiString str;
			material->GetTexture(type, i, &str);

			auto texture = TextureLoader::Load(str.C_Str(), "../Data/Textures");

			if(texture)
			{
				texture->GenerateMipMaps();
				texture->SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
				texture->SetFilters(Filter::LinearMipmapLinear, Filter::Linear);

				textures.push_back(texture);
			}
		}

		return textures;
	}

	glm::vec3 ModelLoader::ProcessColor(const void *aiMaterial, int colorType)
	{
		const auto material = static_cast<const ::aiMaterial*>(aiMaterial);

		aiColor3D color;
		switch(colorType)
		{
			case COLOR_DIFFUSE: material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				break;
			case COLOR_SPECULAR: material->Get(AI_MATKEY_COLOR_SPECULAR, color);
				break;
			case COLOR_AMBIENT: material->Get(AI_MATKEY_COLOR_AMBIENT, color);
				break;
			default: color = aiColor3D(0, 0, 0);
		}

		const auto result = Convert(color);
		LOG_DEBUG("Processing {}, Color (R: {}, G: {}, B: {})", ToStringColor(colorType), result.r, result.g, result.b);

		return result;
	}
}
