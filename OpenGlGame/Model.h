#pragma once

#include <glm/mat4x4.hpp>

#include "Mesh.h"

namespace Game
{
	class Model
	{
	public:
		using SizeType = size_t;
		using MeshContainerType = std::vector<Pointer<Mesh>>;
	private:
		MeshContainerType m_Meshes;
		glm::mat4 m_Transform = glm::mat4(1.f);

	public:
		Model() = default;
		explicit Model(const MeshContainerType &meshes, const glm::mat4 &transform = glm::mat4(1.f));

		SizeType Count() const { return m_Meshes.size(); }

		const Pointer<Mesh>& GetMesh(SizeType index) const;
		Pointer<Mesh>& GetMesh(SizeType index);

		const Pointer<Mesh>& operator[](SizeType index) const { return GetMesh(index); }
		Pointer<Mesh>& operator[](SizeType index) { return GetMesh(index); }

		void AddMesh(Pointer<Mesh> mesh);
		void RemoveMesh(size_t index);

		void Clear();

		const glm::mat4& GetTransform() const { return m_Transform; }
		void SetTransform(const glm::mat4 &transform) { m_Transform = transform; }

		size_t GetMeshesCount() const { return m_Meshes.size(); }

		MeshContainerType::iterator begin() { return m_Meshes.begin(); }
		MeshContainerType::iterator end() { return m_Meshes.end(); }

		MeshContainerType::const_iterator begin() const { return m_Meshes.begin(); }
		MeshContainerType::const_iterator end() const { return m_Meshes.end(); }
	};
}
