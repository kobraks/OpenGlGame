#pragma once

#include <vector>

#include "VertexArray.h"
#include "glm/mat4x4.hpp"
#include "Vertex.h"
#include "Material.h"

namespace Game
{
	class Model;

	class Mesh
	{
		friend class Model;

		glm::mat4 m_Transform              = glm::mat4(1.f);
		Pointer<VertexArray> m_VertexArray = nullptr;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Pointer<Material> m_Material;

		std::string m_Name;

	public:
		Mesh() = delete;
		Mesh(
			const std::string &name,
			const std::vector<Vertex> &vertices,
			const std::vector<uint32_t> &indices,
			Pointer<Material> material = nullptr,
			const glm::mat4 &transform = glm::mat4(1.f)
			);
		Mesh(
			const std::vector<Vertex> &vertices,
			const std::vector<uint32_t> &indices,
			Pointer<Material> material = nullptr,
			const glm::mat4 &transform = glm::mat4(1.f)
			);

		const glm::mat4& GetTransform() const { return m_Transform; }
		const Pointer<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		
		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

		const std::string& GetName() const { return m_Name; }

		Pointer<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Pointer<Material> material) { m_Material = material; }
	private:
		Mesh(const void *aiMesh);

		void Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const glm::mat4 &transform = glm::mat4(1.f));
		void CreateVertexArray(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
	};
}
