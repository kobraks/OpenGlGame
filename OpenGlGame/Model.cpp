#include "pch.h"
#include "Model.h"

namespace Game
{
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
