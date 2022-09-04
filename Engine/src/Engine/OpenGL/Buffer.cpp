#include "pch.h"
#include "Engine/OpenGL/Buffer.h"

#include "Engine/OpenGL/ShaderProgram.h"

namespace Game
{
	constexpr static std::string_view ToString(BufferType type)
	{
		switch(type)
		{
			case BufferType::Index:
				return "Index buffer";
			case BufferType::Vertex:
				return "Vertex buffer";
			case BufferType::Uniform:
				return "Uniform buffer";
			default:
				return "Unknown";
		}
	}

	void BufferLayout::CalculateOffsetAndStride()
	{
		size_t offset = 0;
		m_Stride      = 0;

		for(auto &element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	BufferObject::Internals::Internals(BufferType type) : Type(type)
	{
		glCreateBuffers(1, &ID);
		GL_LOG_DEBUG("Creating {}: {}", ToString(Type), ID);
	}

	BufferObject::Internals::~Internals()
	{
		glDeleteBuffers(1, &ID);
		GL_LOG_DEBUG("Deleteing {}: {}", ToString(Type), ID);
	}

	void BufferObject::Internals::Data(BufferUsage usage, uint32_t size, const void *data)
	{
		Usage = usage;
		GL_LOG_DEBUG("{} (id: {}) -> size: {}", ToString(Type), ID, size);

		glNamedBufferData(ID, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
		Size = size;
	}

	void BufferObject::Internals::SubData(uint32_t size, uint32_t offset, const void *data)
	{
		ASSERT(offset + size <= Size, "Out of range");

		if(offset + size > Size)
			throw std::out_of_range("Out of range");

		glNamedBufferSubData(ID, offset, size, data);

		GL_LOG_DEBUG("{} (id: {}) -> SubData: Size: {}, offset: {}", ToString(Type), ID, size, offset);
	}

	void BufferObject::Internals::Bind() const
	{
		glBindBuffer(static_cast<GLenum>(Type), ID);
	}

	void BufferObject::Internals::UnBind() const
	{
		glBindBuffer(static_cast<GLenum>(Type), 0);
	}

	BufferObject::BufferObject(BufferType type) : m_Internals(MakePointer<Internals>(type)){}

	Pointer<BufferContent> BufferObject::GetContent(BufferAccess access) const
	{
		return Pointer<BufferContent>(new BufferContent(access, *this));
	}

	BufferContent::BufferContent(BufferAccess access, const BufferObject &object) : m_Buffer(object), m_Access(access)
	{
		ASSERT(object.Size() != 0, "Attempting to get access in to uninitialized memory");

		if (object.Size() == 0)
			throw std::runtime_error("Attempting to get access in to uninitialized memory");

		glMapNamedBuffer(object.ID(), static_cast<GLenum>(m_Access));
	}

	BufferContent::~BufferContent()
	{
		glUnmapBuffer(m_Buffer.ID());
	}

	const void* BufferContent::Get() const
	{
		ASSERT(m_Access != BufferAccess::WriteOnly, "No read access");

		if (m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}

	void* BufferContent::Get()
	{
		ASSERT(m_Access != BufferAccess::WriteOnly, "No read access");

		if (m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}
	void BufferContent::Set(const void *data, size_t size, size_t offset)
	{
		ASSERT(m_Access != BufferAccess::ReadOnly, "No write access");
		ASSERT(size + offset < m_Buffer.Size(), "Data povided is too bing");

		if (m_Access == BufferAccess::ReadOnly)
			throw std::runtime_error("No write access");

		if (size + offset > m_Buffer.Size())
			throw std::out_of_range("Data size provided is too bing");

		std::memcpy(static_cast<char*>(m_Data) + offset, data, size);
	}

	VertexBuffer::VertexBuffer(uint32_t size) : BufferObject(BufferType::Vertex)
	{
		m_internals = MakePointer<Internals>();
		Data(BufferUsage::DynamicDraw, size);
	}

	VertexBuffer::VertexBuffer(const void *vertices, uint32_t size) : BufferObject(BufferType::Vertex)
	{
		m_internals = MakePointer<Internals>();
		Data(BufferUsage::DynamicDraw, size, vertices);
	}

	void VertexBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
	{
		SubData(size, offset, data);
	}

	IndexBuffer::IndexBuffer(const uint32_t *indices, uint32_t count) : BufferObject(BufferType::Index)
	{
		m_Internals = MakePointer<Internals>();
		m_Internals->Count = count;
		Data(BufferUsage::StaticDraw, count * sizeof(uint32_t), indices);
	}

	UniformBuffer::Internals::Internals(uint32_t size, BufferUsage usage)
	{
		Data = new char[size];
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
		if (offset + size > Size())
			throw std::out_of_range("Out of Range");

		std::memcpy(m_Internals->Data + offset, data, size);
		SubData(size, offset, data);
	}

	void UniformBuffer::Set(glm::vec2 &value, size_t offset)
	{
		Set(&value.x, sizeof(glm::vec2), offset);
	}

	void UniformBuffer::Set(glm::vec3 &value, size_t offset)
	{
		Set(&value.x, sizeof(glm::vec3), offset);
	}

	void UniformBuffer::Set(glm::vec4 &value, size_t offset)
	{
		Set(&value.x, sizeof(glm::vec4), offset);
	}

	void UniformBuffer::Set(glm::mat3 &value, size_t offset)
	{
		for (uint32_t i = 0; i < 3; ++i)
			Set(value[i], offset + i * sizeof(glm::vec3));
	}

	void UniformBuffer::Set(glm::mat4 &value, size_t offset)
	{
		for (uint32_t i = 0; i < 4; ++i)
			Set(value[i], offset + i * sizeof(glm::vec4));
	}
}
