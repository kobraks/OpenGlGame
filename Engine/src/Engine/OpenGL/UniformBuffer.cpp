#include "pch.h"
#include "Engine/OpenGL/UniformBuffer.h"
#include "Engine/OpenGL/ShaderProgram.h"

namespace Game
{
	UniformBuffer::Internals::Internals(uint32_t size, BufferUsage usage)
	{
		Data  = new char[size];
		Usage = usage;
	}

	UniformBuffer::Internals::~Internals()
	{
		delete[] Data;
	}

	UniformBuffer::UniformBuffer(
		const ShaderProgram &program,
		const std::string &name,
		BufferUsage usage
		) : BufferObject(BufferType::Uniform)
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
		for(uint32_t i = 0; i < 3; ++i)
			Set(value[i], offset + i * sizeof(glm::vec3));
	}

	void UniformBuffer::Set(glm::mat4 &value, size_t offset)
	{
		for(uint32_t i = 0; i < 4; ++i)
			Set(value[i], offset + i * sizeof(glm::vec4));
	}
}
