#include "pch.h"
#include "BufferObject.h"

#include <stdexcept>

#include "Assert.h"
#include "Log.h"
#include "GLCheck.h"

namespace Game
{
	BufferObject::Internals::Internals()
	{
		glGenBuffers(1, &Id);
	}

	BufferObject::Internals::Internals(IdType id)
	{
		m_ProvidedId = true;
		Id = id;
	}

	BufferObject::Internals::~Internals()
	{
		if (!m_ProvidedId)
			glDeleteBuffers(1, &Id);
	}

	BufferObject::BufferObject(BufferType type, bool def)
	{
		m_Internals       = MakePointer<Internals>(0);
		m_Internals->Type = type;
	}

	BufferObject::BufferObject(const BufferType type, const BufferUsage usage)
	{
		m_Internals        = MakePointer<Internals>();
		m_Internals->Type  = type;
		m_Internals->Usage = usage;

		GL_LOG_INFO("Generating {} buffer {}, usage: {}", GetBufferTypeAsString(type), m_Internals->Id, GetBufferUsageAsString(usage));
	}

	void BufferObject::SendData(const void *data, const size_t size) const
	{
		if(size > m_Internals->Size)
		{
			m_Internals->Size = size;
			GL_CHECK(glNamedBufferData(m_Internals->Id, size, data, static_cast<GLenum>(m_Internals->Usage)));
			// GL_CHECK(glBufferData(static_cast<uint32_t>(m_Type), size, data, static_cast<uint32_t>(m_Usage)));
		}
		else
			SendSubData(data, size, 0);
	}

	void BufferObject::SendSubData(const void *data, const size_t size, const size_t offset) const
	{
		ASSERT((size + offset) < m_Internals->Size, "Updating chunk of data is to big, buffer size: {}, requested space: {}", size, offset);

		if(m_Internals->Size < size + offset)
			throw std::runtime_error(fmt::format("Updating chunk of data is to big, buffer size: {}, requested space: {}", size, offset));

		GL_CHECK(glNamedBufferSubData(m_Internals->Id, offset, size, data));
		// GL_CHECK(glBufferSubData(static_cast<uint32_t>(m_Type), offset, size, data));
	}

	void BufferObject::Allocate(const size_t size)
	{
		if(size > m_Internals->Size)
			SendData(nullptr, size);
	}

	void BufferObject::ReAlloc(size_t size)
	{
		SendData(nullptr, size);
	}

	BufferObject::operator unsigned() const
	{
		return m_Internals->Id;
	}
	BufferObject::IdType BufferObject::Id() const
	{
		return m_Internals->Id;
	}
	
	BufferUsage BufferObject::Usage() const
	{
		return m_Internals->Usage;
	}
	BufferType BufferObject::Type() const
	{
		return m_Internals->Type;
	}

	size_t BufferObject::Size() const
	{
		return m_Internals->Size;
	}

	void BufferObject::Bind() const
	{
		GL_CHECK(glBindBuffer(static_cast<GLenum>(m_Internals->Type), m_Internals->Id));

		if(m_Internals->Changed)
		{
			m_Internals->Changed = false;
			SendValues();
		}
	}

	void BufferObject::UnBind() const
	{
		GetDefault(m_Internals->Type)->Bind();
	}

	bool BufferObject::IsChanged() const
	{
		return m_Internals->Changed;
	}

	void BufferObject::MarkAsChanged() const
	{
		m_Internals->Changed = true;
	}

	BufferObject* BufferObject::GetDefault(BufferType type)
	{
		if(!m_Initialized)
			Init();

		return &m_Buffers.at(type);
	}

	void BufferObject::Flush() const
	{
		SendValues();
	}

	void BufferObject::GetData(void *data, const size_t offset, const size_t length) const
	{
		GL_CHECK(glGetNamedBufferSubData(m_Internals->Id, offset, length, data));
	}

	Pointer<BufferObject> BufferObject::Clone() const
	{
		auto object = Pointer<BufferObject>(new BufferObject(Type(), Usage()));

		object->Allocate(Size());
		GL_CHECK(glCopyBufferSubData(*this, *object, 0, 0, Size()));
		GL_LOG_INFO(
		            "Coping contents of {} buffer (id: {}) to {} buffer (id: {})",
		            GetBufferTypeAsString(Type()),
		            Id(),
		            GetBufferTypeAsString(object->Type()),
		            object->Id()
		           );

		return object;
	}

	Pointer<BufferContent> BufferObject::GetContent(const BufferAccess access) const
	{
		return Pointer<BufferContent>(new BufferContent(access, *this));
	}

	void BufferObject::Init()
	{
		m_Initialized = false;

		m_Buffers.emplace(BufferType::Vertex, BufferObject(BufferType::Vertex, true));
		m_Buffers.emplace(BufferType::Uniform, BufferObject(BufferType::Uniform, true));
		m_Buffers.emplace(BufferType::Index, BufferObject(BufferType::Index, true));
	}

	BufferContent::BufferContent(const BufferAccess access, const BufferObject &buffer) : m_Buffer(buffer),
	                                                                                      m_Access(access)
	{
		ASSERT(buffer.Size() != 0, "Attempting to get access to uninitialized memory");
		if(buffer.Size() == 0)
			throw std::runtime_error("Attempting to get access to uninitialized memory");

		GL_CHECK(m_Data = glMapNamedBuffer(buffer.Id(), static_cast<GLenum>(m_Access)));
	}

	BufferContent::~BufferContent()
	{
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
		ASSERT(size < m_Buffer.Size(), "Data provided is too big");

		if(m_Access == BufferAccess::ReadOnly)
			throw std::runtime_error("No write access");

		if(size >= m_Buffer.Size())
			throw std::out_of_range("Data size provided is too big");

		std::memcpy(static_cast<char*>(m_Data) + offset, data, size);
	}
}
