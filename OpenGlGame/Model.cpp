#include "pch.h"
#include "Model.h"

#include <unordered_map>
#include <memory>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#define ASSIMP_FLAGS\
	aiProcess_JoinIdenticalVertices |\
	aiProcess_Triangulate |\
	aiProcess_PreTransformVertices |\
	aiProcess_GenNormals |\
	aiProcess_GenUVCoords |\
	aiProcess_OptimizeMeshes |\
	aiProcess_FixInfacingNormals

namespace Game
{
	static std::unordered_map<std::string, Pointer<Model>> s_Models;

	Pointer<Model> Model::Load(const std::string &fileName, const std::string &directory)
	{
		const auto iter = s_Models.find(fileName);

		const std::string name = directory + "/" + fileName;

		if(iter != s_Models.end())
		{
			LOG_INFO("\"{}\" model already loaded", fileName);
			return iter->second;
		}

		LOG_INFO("Opening \"{}\"", name);
		auto importer = MakePointer<Assimp::Importer>();

		auto *const scene = importer->ReadFile(name, ASSIMP_FLAGS);

		if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("Unable to import \"{}\", {}", name, importer->GetErrorString());
			return nullptr;
		}

		LOG_INFO("Scene imported sucessfuly");
		const auto meshes = scene->mNumMeshes;
		LOG_INFO("Meshes count: {}", meshes);

		MeshContainerType modelMeshes(meshes);
		for(uint32_t i = 0; i < meshes; ++i)
		{
			LOG_INFO("Processing mesh number: {}", i);
			auto *const mesh = scene->mMeshes[i];
			modelMeshes[i]   = ProcessMesh(mesh, scene);
		}

		Pointer<Model> model = MakePointer<Model>();
		model->m_Meshes      = modelMeshes;

		s_Models[fileName] = model;

		return model;
	}

	const Pointer<Mesh>& Model::GetMesh(SizeType index) const
	{
		ASSERT(index < m_Meshes.size(), "Out of range");
		if(index >= m_Meshes.size())
			throw std::out_of_range("Out of range");

		return m_Meshes[index];
	}

	Pointer<Mesh>& Model::GetMesh(SizeType index)
	{
		ASSERT(index < m_Meshes.size(), "Out of range");
		if(index >= m_Meshes.size())
			throw std::out_of_range("Out of range");

		return m_Meshes[index];
	}


	void Model::AddMesh(Pointer<Mesh> mesh)
	{
		m_Meshes.push_back(mesh);
	}

	void Model::Clear()
	{
		m_Meshes.clear();
	}

	void Model::ClearCashed()
	{
		s_Models.clear();
	}

	Pointer<Material> Model::ProcessMaterial(const void *aiMesh, const void *aiScene)
	{
		auto *const mesh  = static_cast<const ::aiMesh*>(aiMesh);
		auto *const scene = static_cast<const ::aiScene*>(aiScene);

		auto *const material = scene->mMaterials[mesh->mMaterialIndex];
		LOG_INFO("Processing material {}", mesh->mMaterialIndex);

		return Material::ProcessMaterial(material);
	}

	Pointer<Mesh> Model::ProcessMesh(const void *aiMesh, const void *aiScene)
	{
		auto mesh = new Mesh(aiMesh);
		mesh->SetMaterial(ProcessMaterial(aiMesh, aiScene));
		return Pointer<Mesh>(mesh);
	}
}
