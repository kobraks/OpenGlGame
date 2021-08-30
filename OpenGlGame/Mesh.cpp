#include "pch.h"
#include "Mesh.h"

#include "Buffer.h"
#include "OpenGlFunctions.h"

namespace Game
{
	Mesh::Mesh(const std::string &name, uint64_t verticesCount, uint64_t indicesCount, const glm::mat4 &transform) : m_Transform(transform), m_Name(name)
	{
		Reserve(verticesCount, indicesCount);
	}

	Mesh::Mesh(
		const std::string &name,
		const std::vector<Vertex> &vertices,
		const std::vector<uint32_t> &indices,
		Pointer<Material> material,
		const glm::mat4 &transform
		) : m_Transform(transform),
		    m_Vertices(vertices),
		    m_Indices(indices),
		    m_Material(material),
		    m_Name(name)
	{
		Create(vertices, indices);
	}

	void Mesh::Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
	{
		m_VertexArray = MakePointer<VertexArray>();

		auto vertexBuffer = MakePointer<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
		vertexBuffer->SetLayout(Vertex::GetLayout());

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(MakePointer<IndexBuffer>(indices.data(), indices.size()));

		m_Ready = true;
	}

	void Mesh::Reserve(uint64_t verticesCount, uint64_t indicesCount)
	{
		m_VertexArray = MakePointer<VertexArray>();

		auto vertexBuffer = MakePointer<VertexBuffer>(nullptr, sizeof(Vertex) * verticesCount);
		vertexBuffer->SetLayout(Vertex::GetLayout());

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(MakePointer<IndexBuffer>(nullptr, indicesCount));

		m_Ready = false;
	}
}
