#include "pch.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Engine {
	void IndexBuffer::SetIndices(const uint32_t* data, uint32_t elementCount, uint32_t offset) {
		ENGINE_ASSERT((offset + elementCount * sizeof(uint32_t)) <= Size(), "IndexBuffer::SetIndices out of bounds");

		m_Count = elementCount;
		Write({ data,  elementCount * sizeof(uint32_t)}, offset);
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::span<const uint32_t> indices, BufferStorageFlags flags) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(indices.data(), static_cast<uint32_t>(indices.size_bytes()), flags);
		buffer->m_Count = static_cast<uint32_t>(indices.size());

		return buffer;
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::span<const uint32_t> indices, BufferUsage usageHint) {
		auto buffer = Ref<IndexBuffer>(new IndexBuffer());
		buffer->Allocate(indices.data(), static_cast<uint32_t>(indices.size_bytes()), usageHint);
		buffer->m_Count = static_cast<uint32_t>(indices.size());

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

	IndexBuffer::IndexBuffer() : BufferObject(BufferTarget::ElementArray) {
	}

}