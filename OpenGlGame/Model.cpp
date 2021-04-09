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

	Model::Model(const MeshContainerType &meshes, const glm::mat4 &transform)
	{
		m_Meshes = meshes;
		m_Transform = transform;
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

	void Model::RemoveMesh(size_t index)
	{
		ASSERT(index < m_Meshes.size(), "Out of range");
		if (index >= m_Meshes.size())
			throw std::out_of_range("Out of range");

		m_Meshes.erase(m_Meshes.begin() + index);
	}

	void Model::Clear()
	{
		m_Meshes.clear();
	}
}
