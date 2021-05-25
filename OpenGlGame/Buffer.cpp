#include "pch.h"
#include "Buffer.h"

#include "GLCheck.h"
#include "glad/glad.h"
#include "ShaderProgram.h"

namespace Game
{
	constexpr static std::string_view ToString(BufferType type)
	{
		switch(type)
		{
			case BufferType::Index: return "Index buffer";
			case BufferType::Vertex: return "Vertex buffer";
			case BufferType::Uniform: return "Uniform buffer";
			default: return "Unknown";
		}
	}

	BufferContent::BufferContent(const BufferAccess access, const BufferObject &buffer) : m_Buffer(buffer),
	                                                                                      m_Access(access)
	{
		ASSERT(buffer.Size() != 0, "Attempting to get access to uninitialized memory");

		if(buffer.Size() == 0)
			throw std::runtime_error("Attempting to get access to uninitialized memory");

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(m_Data = glMapNamedBuffer(buffer.Id(), static_cast<GLenum>(m_Access)));
	}

	BufferContent::~BufferContent()
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glUnmapNamedBuffer(m_Buffer.Id()));
	}

	const void* BufferContent::Get() const
	{
		ASSERT(m_Access == BufferAccess::ReadOnly, "No read access");

		if(m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}

	void* BufferContent::Get()
	{
		ASSERT(m_Access == BufferAccess::ReadOnly, "No read access");

		if(m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}

	void BufferContent::Set(const void *data, const size_t size, const size_t offset)
	{
		ASSERT(m_Access == BufferAccess::WriteOnly, "No write access");
		ASSERT(size + offset < m_Buffer.Size(), "Data provided is too big");

		if(m_Access == BufferAccess::ReadOnly)
			throw std::runtime_error("No write access");

		if(size + offset >= m_Buffer.Size())
			throw std::out_of_range("Data size provided is too big");

		std::memcpy(static_cast<char*>(m_Data) + offset, data, size);
	}

	BufferObject::Internals::Internals(BufferType type) : Type(type)
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glCreateBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Creating {}: {}", ToString(type), Id);
	}

	BufferObject::Internals::~Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glDeleteBuffers(1, &Id));
		OPENGL_LOG_DEBUG("Deleting {}: {}", ToString(Type), Id);
	}

	void BufferObject::Internals::Data(BufferUsage usage, uint32_t size, const void *data)
	{
		Usage = usage;
		OPENGL_LOG_DEBUG("{} (id: {}) -> Data: Size: {}", ToString(Type), Id, size);

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glNamedBufferData(Id, size, data, static_cast<GLenum>(usage)));
		Size = size;
	}

	void BufferObject::Internals::SubData(uint32_t size, uint32_t offset, const void *data)
	{
		ASSERT(offset + size <= Size, "Out of range");

		if(offset + size > Size)
			throw std::out_of_range("Out of range");

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glNamedBufferSubData(Id, offset, size, data));

		OPENGL_LOG_DEBUG("{} (id: {}) -> SubData: Size: {}, Offset: {}", ToString(Type), Id, size, offset);
	}

	void BufferObject::Internals::Bind() const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindBuffer(static_cast<GLenum>(Type), Id));
	}

	void BufferObject::Internals::Unbind() const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindBuffer(static_cast<GLenum>(Type), 0));
	}

	BufferObject::BufferObject(BufferType type) : m_Internals(MakePointer<Internals>(type)) {}

	Pointer<BufferContent> BufferObject::GetContent(BufferAccess access) const
	{
		BufferContent *content = new BufferContent(access, *this);

		return Pointer<BufferContent>(content);
	}

	VertexBuffer::VertexBuffer(uint32_t size) : BufferObject(BufferType::Vertex)
	{
		m_Internals = MakePointer<Internals>();
		Data(BufferUsage::DynamicDraw, size);
	}

	VertexBuffer::VertexBuffer(const void *vertices, uint32_t size) : BufferObject(BufferType::Vertex)
	{
		m_Internals = MakePointer<Internals>();
		Data(BufferUsage::StaticDraw, size, vertices);
	}

	void VertexBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		SubData(size, offset, data);
	}

	IndexBuffer::IndexBuffer(const uint32_t *indices, uint32_t count) : BufferObject(BufferType::Index)
	{
		m_Internals        = MakePointer<Internals>();
		m_Internals->Count = count;
		Data(BufferUsage::StaticDraw, count * sizeof(uint32_t), indices);
	}

	UniformBuffer::Internals::Internals(uint32_t size, BufferUsage usage)
	{
		Data  = new char[size];
		Usage = usage;
	}

	UniformBuffer::Internals::~Internals()
	{
		delete[] Data;
	}

	UniformBuffer::UniformBuffer(const ShaderProgram &program, const std::string &name, BufferUsage usage) : BufferObject(BufferType::Uniform)
	{
		auto info = program.QueryUniformBlock(name);

		m_Internals = MakePointer<Internals>(info.Size, usage);
		Data(usage, info.Size, nullptr);
	}

	UniformBuffer::UniformBuffer(uint32_t size, BufferUsage usage) : BufferObject(BufferType::Uniform)
	{
		m_Internals = MakePointer<Internals>(size, usage);
		Data(usage, size, nullptr);
	}

	UniformBuffer::UniformBuffer(const void *data, uint32_t size, BufferUsage usage) : BufferObject(BufferType::Uniform)
	{
		m_Internals = MakePointer<Internals>(size, usage);

		Data(usage, size, data);
		std::memcpy(m_Internals->Data, data, size);
	}

	void UniformBuffer::Usage(BufferUsage usage)
	{
		Data(usage, Size(), m_Internals->Data);
		m_Internals->Usage = usage;
	}

	void UniformBuffer::Set(const void *data, size_t size, size_t offset)
	{
		ASSERT(offset + size <= Size(), "Out of range");
		if(offset + size > Size())
			throw std::out_of_range("Out of Range");

		std::memcpy(m_Internals->Data + offset, data, size);
		SubData(size, offset, data);
	}

	void UniformBuffer::Set(const glm::mat3 &value, size_t offset)
	{
		for(uint32_t i = 0; i < 3; ++i)
			Set(value[i], offset + i * sizeof(glm::vec4));
	}

	void UniformBuffer::Set(const glm::vec3 &value, size_t offset)
	{
		Set(&value, sizeof(glm::vec3), offset);
	}

	void UniformBuffer::Set(const glm::mat4 &value, size_t offset)
	{
		for(uint32_t i = 0; i < 4; ++i)
			Set(value[i], offset + i * sizeof(glm::vec4));
	}
	
	void UniformBuffer::Set(const glm::vec4 &value, size_t offset)
	{
		Set(&value, sizeof(glm::vec4), offset);
	}
}
