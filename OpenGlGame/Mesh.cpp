#include "pch.h"
#include "Mesh.h"

#include "Buffer.h"

namespace Game
{
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
	}
}
