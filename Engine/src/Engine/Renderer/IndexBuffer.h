#pragma once
#include "Engine/Renderer/BufferObject.h"

namespace Engine {
	class IndexBuffer : public BufferObject {
	public:
		void SetIndices(const uint32_t* data, uint32_t elementCount, uint32_t offset = 0);

		uint32_t Count() const { return m_Count; }

		static Ref<IndexBuffer> Create(std::span<const uint32_t> indices, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<IndexBuffer> Create(std::span<const uint32_t> indices, BufferUsage usageHint);
		static Ref<IndexBuffer> Create(const uint32_t* data, uint32_t elementCount, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<IndexBuffer> Create(const uint32_t* data, uint32_t elementCount, BufferUsage usageHint);

		static Ref<IndexBuffer> Create(const BufferView& view, BufferStorageFlags flags = BufferStorageFlags::None) {
			return Create(view.AsSpan<uint32_t>(), flags);
		}

		static Ref<IndexBuffer> Create(const BufferView& view, BufferUsage usageHint) {
			return Create(view.AsSpan<uint32_t>(), usageHint);
		}
	private:
		IndexBuffer();

		uint32_t m_Count = 0;
	};
}