#include "pch.h"
#include "Engine/OpenGL/VertexBuffer.h"

namespace Game
{
	VertexBuffer::VertexBuffer(size_t size, BufferUsage usage) : BufferObject(BufferType::Vertex)
	{
		m_Internals = MakePointer<Internals>();
		Data(usage, size);
	}

	VertexBuffer::VertexBuffer(const void *vertices, size_t size, BufferUsage usage) : BufferObject(BufferType::Vertex)
	{
		Data(usage, size, vertices);
	}

	void VertexBuffer::SetData(const void *data, size_t size, size_t offset)
	{
		SubData(size, offset, data);
	}

	void VertexBuffer::Realloc(size_t size, const void *data, BufferUsage usage)
	{
		Data(usage, size, data);
	}
}
