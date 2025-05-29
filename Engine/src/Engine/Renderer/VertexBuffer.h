#pragma once
#include "Engine/Renderer/BufferObject.h"
#include "Engine/Renderer/BufferLayout.h"
#include "Engine/Renderer/Vertex.h"

namespace Engine {
	class VertexBuffer : public BufferObject {
	public:
		virtual ~VertexBuffer() override = default;

		void SetData(const std::byte* data, uint32_t size, uint32_t offset = 0) {
			Upload({ data, size }, offset);
		}

		const BufferLayout& GetLayout() const { return *m_Layout; }
		void SetLayout(const BufferLayout& layout) { m_Layout = MakeScope<BufferLayout>(layout); };

		static Ref<VertexBuffer> Create(std::span<Vertex> vertices, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<VertexBuffer> Create(std::span<Vertex> vertices, BufferUsage usageHint);
		static Ref<VertexBuffer> Create(uint32_t size, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<VertexBuffer> Create(uint32_t size, BufferUsage usageHint);
	protected:
		VertexBuffer();

	private:
		Scope<BufferLayout> m_Layout;
	};
}