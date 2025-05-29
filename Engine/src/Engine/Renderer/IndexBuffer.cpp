#include "pch.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Engine {
	Ref<IndexBuffer> IndexBuffer::Create(std::span<uint32_t> indices, BufferStorageFlags flags) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(indices.data(), indices.size_bytes(), flags);
		buffer->m_Count = indices.size();

		return buffer;
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::span<uint32_t> indices, BufferUsage usageHint) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(indices.data(), indices.size_bytes(), usageHint);
		buffer->m_Count = indices.size();

		return buffer;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* data, uint32_t elementCount, BufferStorageFlags flags) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(data, elementCount * sizeof(uint32_t), flags);
		buffer->m_Count = elementCount;

		return buffer;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* data, uint32_t elementCount, BufferUsage usageHint) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(data, elementCount, usageHint);
		buffer->m_Count = elementCount;

		return buffer;
	}

	IndexBuffer::IndexBuffer() : BufferObject(GL_ELEMENT_ARRAY_BUFFER) {
	}

}