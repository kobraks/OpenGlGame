#include "pch.h"
#include "Buffer.h"


#include "GLCheck.h"
#include "glad/glad.h"

namespace Game
{
	void VertexBuffer::Internals::Data(uint32_t size, const void *data)
	{
		GL_CHECK(glNamedBufferData(Id, size, data, GL_DYNAMIC_DRAW));
		Size = size;

		OPENGL_LOG_DEBUG("Vertex buffer (id: {}) -> Data: Size: {}", Id, size);
	}

	void VertexBuffer::Internals::SubData(uint32_t size, uint32_t offset, const void *data)
	{
		ASSERT(offset + size < Size, "Out of range");

		if (offset + size > Size)
			std::out_of_range("Out of buffer range");
		
		GL_CHECK(glNamedBufferSubData(Id, offset, size, data));

		OPENGL_LOG_DEBUG("Vertex buffer (id: {}) -> SubData: Size: {}, Offset: {}", Id, size, offset);
	}

	void VertexBuffer::Internals::Bind() const
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, Id));
	}

	void VertexBuffer::Internals::Unbind() const
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	VertexBuffer::Internals::Internals()
	{
		GL_CHECK(glCreateBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Creating vertexbuffer: {}", Id);
	}

	VertexBuffer::Internals::~Internals()
	{
		GL_CHECK(glDeleteBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Removing vertexBuffer: {}", Id);
		Id = 0;
	}

	VertexBuffer::VertexBuffer(uint32_t size)
	{
		m_Internals = MakePointer<Internals>();
		m_Internals->Data(size, nullptr);
	}

	VertexBuffer::VertexBuffer(const void *vertices, uint32_t size)
	{
		m_Internals = MakePointer<Internals>();
		m_Internals->Data(size, vertices);
	}

	void VertexBuffer::Bind() const
	{
		m_Internals->Bind();
	}

	void VertexBuffer::Unbind() const
	{
		m_Internals->Unbind();
	}
	
	void VertexBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		m_Internals->SubData(size, offset, data);
	}

	void IndexBuffer::Internals::Data(uint32_t count, const void *data)
	{
		GL_CHECK(glNamedBufferData(Id, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
		Count = count;
		OPENGL_LOG_DEBUG("Index buffer (id: {}) -> Data: Count: {}, Size: {}, uint32_t size: {}", Id, count, sizeof(uint32_t) * count, sizeof(uint32_t));
	}
	
	void IndexBuffer::Internals::Bind() const
	{
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id));
	}
	
	void IndexBuffer::Internals::Unbind() const
	{
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	IndexBuffer::Internals::Internals()
	{
		GL_CHECK(glCreateBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Creating index buffer: {}", Id);
	}
	
	IndexBuffer::Internals::~Internals()
	{
		GL_CHECK(glDeleteBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Removing index buffer: {}", Id);
	}
	
	IndexBuffer::IndexBuffer(const uint32_t *indices, uint32_t count)
	{
		m_Internals = MakePointer<Internals>();
		m_Internals->Data(count, indices);
	}
}
