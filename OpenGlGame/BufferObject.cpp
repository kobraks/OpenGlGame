#include "pch.h"
#include "BufferObject.h"


#include "Assert.h"
#include "Log.h"

namespace Game
{
	static void DeleteBuffer(BufferObject::IdType *id)
	{
		glDeleteBuffers(1, id);

		delete id;
	}

	static Pointer<BufferObject::IdType> CreateBuffer()
	{
		auto buffer = Pointer<BufferObject::IdType>(new BufferObject::IdType(), DeleteBuffer);
		glGenBuffers(1, &*buffer);

		return buffer;
	}

	BufferObject::BufferObject(BufferType type, bool def) : m_Usage(BufferUsage::StaticDraw),
	                                                        m_Type(type)
	{
		m_Buffer = MakePointer<IdType>(0);
	}

	BufferObject::BufferObject(const BufferType type, const BufferUsage usage) : m_Usage(usage),
		m_Type(type)
	{
		m_Buffer = CreateBuffer();

		GL_LOG_INFO(
		            "Generating {} buffer {}, usage: {}",
		            GetBufferTypeAsString(type),
		            *m_Buffer,
		            GetBufferUsageAsString(usage)
		           );
	}

	void BufferObject::SendData(const void *data, const size_t size) const
	{
		if(size > m_Size)
		{
			m_Size = size;
			glBufferData(static_cast<uint32_t>(m_Type), size, data, static_cast<uint32_t>(m_Usage));
		}
		else
			SendSubData(data, size, 0);
	}

	void BufferObject::SendSubData(const void *data, const size_t size, const size_t offset) const
	{
		ASSERT((m_Size < size + offset), "Updating chunk of data is to big buffer size: {}, requested space: {}", size, offset);

		if (m_Size < size + offset)
			throw std::runtime_error(fmt::format("Updating chunk of data is to big buffer size: {}, requested space: {}", size, offset));

		glBufferSubData(static_cast<uint32_t>(m_Type), offset, size, data);
	}

	void BufferObject::Allocate(const size_t size)
	{
		glBindBuffer(static_cast<uint32_t>(m_Type), *m_Buffer);

		if(size > m_Size)
			SendData(nullptr, size);
	}

	void BufferObject::Bind() const
	{
		glBindBuffer(static_cast<uint32_t>(m_Type), *m_Buffer);

		if(m_Changed)
		{
			m_Changed = false;
			SendValues();
		}
	}

	void BufferObject::UnBind() const
	{
		GetDefault(m_Type)->Bind();
	}

	BufferObject* BufferObject::GetDefault(BufferType type)
	{
		if(!m_Initialized)
			Init();

		return &m_Buffers.at(type);
	}

	void BufferObject::Flush() const
	{
		Bind();
	}

	void BufferObject::Init()
	{
		m_Initialized = false;

		m_Buffers.emplace(BufferType::Vertex, BufferObject(BufferType::Vertex, true));
		m_Buffers.emplace(BufferType::Uniform, BufferObject(BufferType::Uniform, true));
		m_Buffers.emplace(BufferType::Index, BufferObject(BufferType::Index, true));
	}
}
