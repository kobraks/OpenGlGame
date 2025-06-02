#pragma once
#include "Engine/Renderer/BufferObject.h"

namespace Engine {
	class IndexBuffer : public BufferObject {
	public:
		void SetIndices(const uint32_t* data, uint32_t elementCount, uint32_t offset = 0) {
			ENGINE_ASSERT((offset + elementCount * sizeof(uint32_t)) <= Size(), "IndexBuffer::SetIndices out of bounds");

			m_Count = elementCount;
			Write({ data,  elementCount * sizeof(uint32_t)}, offset);
		}

		uint32_t Count() const { return m_Count; }

		static Ref<IndexBuffer> Create(std::span<uint32_t> indices, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<IndexBuffer> Create(std::span<uint32_t> indices, BufferUsage usageHint);
		static Ref<IndexBuffer> Create(const uint32_t* data, uint32_t elementCount, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<IndexBuffer> Create(const uint32_t* data, uint32_t elementCount, BufferUsage usageHint);

	private:
		IndexBuffer();

		uint32_t m_Count = 0;
	};
}