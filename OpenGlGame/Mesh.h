#pragma once

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "VertexArray.h"
#include "Vertex.h"
#include "Material.h"

namespace Game
{
	class Model;
	class ModelLoader;

	class Mesh
	{
		friend class Model;
		friend class ModelLoader;

		glm::mat4 m_Transform = glm::mat4(1.f);

		Pointer<VertexArray> m_VertexArray = nullptr;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		glm::vec3 m_Size = glm::vec3(0.f);
		
		Pointer<Material> m_Material;

		std::string m_Name;

		bool m_Ready = false;

		Mesh(const std::string &name, uint64_t verticesCount, uint64_t indicesCount, const glm::mat4 &transform = glm::mat4(1.f));
	
	public:
		Mesh() = delete;
		Mesh(
			const std::string &name,
			const std::vector<Vertex> &vertices,
			const std::vector<uint32_t> &indices,
			Pointer<Material> material = nullptr,
			const glm::mat4 &transform = glm::mat4(1.f)
			);
		
		size_t GetVerticesCount() const { return m_Vertices.size(); }
		size_t GetIndicesCount() const { return m_Indices.size(); }

		const glm::mat4& GetTransform() const { return m_Transform; }
		void SetTransform(const glm::mat4 &transform) { m_Transform = transform; }
		const Pointer<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

		const std::string& GetName() const { return m_Name; }

		Pointer<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(const Pointer<Material> &material) { m_Material = material; }

		bool IsReady() const { return m_Ready; }
	
	private:
		void Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

		void Reserve(uint64_t verticesCount, uint64_t indicesCount);
	};
}
