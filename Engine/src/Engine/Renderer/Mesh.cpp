#include "pch.h"
#include "Mesh.h"

namespace Engine {
	Mesh::Mesh(const BufferView& vertexData, const BufferView& indexData) {
		m_IndexBuffer = IndexBuffer::Create(indexData, BufferStorageFlags::None);
		m_VertexBuffer = VertexBuffer::Create(vertexData, BufferStorageFlags::None);

		m_VertexBuffer->SetLayout(Vertex::GetLayout());

		m_VertexArray = VertexArray::Create("Mesh Vertex Array");
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_Vertices.resize(vertexData.Size() / sizeof(Vertex));
		std::memcpy(m_Vertices.data(), vertexData.Data(), vertexData.Size());

		m_Indices.resize(indexData.Size() / sizeof(uint32_t));
		std::memcpy(m_Indices.data(), indexData.Data(), indexData.Size());
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
		m_IndexBuffer = IndexBuffer::Create(indices, BufferStorageFlags::None);
		m_VertexBuffer = VertexBuffer::Create(vertices, BufferStorageFlags::None);

		m_VertexBuffer->SetLayout(Vertex::GetLayout());

		m_VertexArray = VertexArray::Create("Mesh Vertex Array");
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_Vertices = std::move(vertices);
		m_Indices = std::move(indices);
	}

}