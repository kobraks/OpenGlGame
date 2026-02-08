#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Renderer/VertexArray.h"

#include <vector>

namespace Engine {
	class Mesh {
	public:
		Mesh() = default;

		static Ref<Mesh> Create(const BufferView& vertexData, const BufferView& indexData);
		static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

		Ref<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexArray> m_VertexArray;
	};

}