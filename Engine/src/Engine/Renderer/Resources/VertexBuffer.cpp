#include "pch.h"
#include "VertexBuffer.h"

#include <glad/glad.h>

namespace Engine {
	Ref<VertexBuffer> VertexBuffer::Create(std::span<const Vertex> vertices, BufferStorageFlags flags) {
		auto buffer = Ref<VertexBuffer>(new VertexBuffer());
		buffer->Allocate(vertices.data(), static_cast<uint32_t>(vertices.size_bytes()), flags);
		buffer->SetLayout(Vertex::GetLayout());

		return buffer;
	}

	Ref<VertexBuffer> VertexBuffer::Create(std::span<const Vertex> vertices, BufferUsage usageHint) {
		auto buffer = Ref<VertexBuffer>(new VertexBuffer());
		buffer->Allocate(vertices.data(), static_cast<uint32_t>(vertices.size_bytes()), usageHint);
		buffer->SetLayout(Vertex::GetLayout());

		return buffer;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::byte* data, uint32_t size, BufferStorageFlags flags) {
		auto buffer = Ref<VertexBuffer>(new VertexBuffer());
		buffer->Allocate(data, size, flags);

		return buffer;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const std::byte* data, uint32_t size, BufferUsage usageHint) {
		auto buffer = Ref<VertexBuffer>(new VertexBuffer());
		buffer->Allocate(data, size, usageHint);

		return buffer;
	}

	VertexBuffer::VertexBuffer() : BufferObject(BufferTarget::Array), m_Layout(MakeScope<BufferLayout>()) {
	}
}
