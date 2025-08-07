#include "pch.h"
#include "Mesh.h"

namespace Engine {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
		m_IndexBuffer = IndexBuffer::Create(indices, BufferStorageFlags::None);
		m_VertexBuffer = VertexBuffer::Create(vertices, BufferStorageFlags::None);

		m_VertexArray = VertexArray::Create("Mesh Vertex Array");
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_VertexBuffer->SetLayout(Vertex::GetLayout());

		m_Vertices = std::move(vertices);
		m_Indices = std::move(indices);
	}

}