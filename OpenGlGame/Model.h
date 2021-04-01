#pragma once

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

		static Pointer<Model> LoadModel(const std::string &name);

		SizeType Count() const { return m_Meshes.size(); }
		
		const Pointer<Mesh>& GetMesh(SizeType index) const;
		Pointer<Mesh>& GetMesh(SizeType index);

		const Pointer<Mesh>& operator[](SizeType index) const { return GetMesh(index); }
		Pointer<Mesh>& operator[](SizeType index) { return GetMesh(index); }

		void AddMesh(Pointer<Mesh> mesh);

		void Clear();

		const glm::mat4& GetTransform() const { return m_Transform; }
	private:
		static Pointer<Material> ProcessMaterial(const void *aiMesh, const void *aiScene);
		
		static Pointer<Mesh> ProcessMesh(const void *aiMesh, const void *aiScene);
	};
}
