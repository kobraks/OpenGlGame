#include "pch.h"
#include "Mesh.h"

namespace Engine {
	Ref<Mesh> Mesh::Create(const BufferView& vertexData, const BufferView& indexData) {
		auto mesh = MakeRef<Mesh>();

		mesh->m_IndexBuffer = IndexBuffer::Create(indexData, BufferStorageFlags::None);
		mesh->m_VertexBuffer = VertexBuffer::Create(vertexData, BufferStorageFlags::None);

		mesh->m_VertexBuffer->SetLayout(Vertex::GetLayout());
		mesh->m_VertexArray = VertexArray::Create("Mesh Vertex Array");
		mesh->m_VertexArray->SetIndexBuffer(mesh->m_IndexBuffer);
		mesh->m_VertexArray->AddVertexBuffer(mesh->m_VertexBuffer);

		mesh->m_Vertices.resize(vertexData.Size() / sizeof(Vertex));
		std::memcpy(mesh->m_Vertices.data(), vertexData.Data(), vertexData.Size());

		mesh->m_Indices.resize(indexData.Size() / sizeof(uint32_t));
		std::memcpy(mesh->m_Indices.data(), indexData.Data(), indexData.Size());

		return mesh;
	}

	Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
		auto mesh = MakeRef<Mesh>();

		mesh->m_IndexBuffer = IndexBuffer::Create(indices, BufferStorageFlags::None);
		mesh->m_VertexBuffer = VertexBuffer::Create(vertices, BufferStorageFlags::None);

		mesh->m_VertexBuffer->SetLayout(Vertex::GetLayout());

		mesh->m_VertexArray = VertexArray::Create("Mesh Vertex Array");
		mesh->m_VertexArray->SetIndexBuffer(mesh->m_IndexBuffer);
		mesh->m_VertexArray->AddVertexBuffer(mesh->m_VertexBuffer);

		mesh->m_Vertices = vertices;
		mesh->m_Indices = indices;

		return mesh;
	}
}
