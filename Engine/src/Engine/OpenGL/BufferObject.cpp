#include "pch.h"
#include "Engine/OpenGL/BufferObject.h"

#include "Engine/OpenGL/ShaderProgram.h"
#include "Engine/Renderer/Context.h"



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

	BufferObject::Internals::Internals(BufferType type) : Type(type)
	{
		Functions = Context::GetContext()->GetFunctions();
		GL_LOG_DEBUG("");
		Buffer = Functions.CreateBuffer();
		GL_LOG_DEBUG("Creating {}: {}", ToString(Type), Buffer);
	}

	BufferObject::Internals::~Internals()
	{
		GL_LOG_DEBUG("Deleteing {}: {}", ToString(Type), Buffer);
		Functions.DeleteBuffer(Buffer);
		if (!Map.expired())
		{
			Map.lock()->Invalidate();
		}
	}

	void BufferObject::Internals::Data(BufferUsage usage, size_t size, const void *data)
	{
		Usage = usage;
		GL_LOG_DEBUG("{} (id: {}) -> size: {}", ToString(Type), Buffer, size);

		Functions.BufferData(Buffer, usage, size, data);
		Size = size;
	}

	void BufferObject::Internals::SubData(size_t size, size_t offset, const void *data)
	{
		ASSERT(offset + size <= Size, "Out of range");

		if(offset + size > Size)
			throw std::out_of_range("Out of range");
		Functions.BufferSubData(Buffer, offset, size, data);

		GL_LOG_DEBUG("{} (id: {}) -> SubData: Size: {}, offset: {}", ToString(Type), Buffer, size, offset);
	}

	void BufferObject::Internals::Bind() const
	{
		Functions.BindBuffer(Buffer ,Type);
	}

	void BufferObject::Internals::UnBind() const
	{
		Functions.BindBuffer(0, Type);
	}

	void * BufferObject::Internals::MapBuffer(BufferAccess access) const
	{
		return Functions.MapBuffer(Buffer, access);
	}

	void BufferObject::Internals::UnMapBuffer() const
	{
		Functions.UnMapBuffer(Buffer);
	}

	OpenGlFunctions & BufferObject::GetFunctions() const
	{
		return m_Internals->Functions;
	}

	BufferObject::BufferObject(BufferType type) : m_Internals(MakePointer<Internals>(type)) {}

	Pointer<BufferContent> BufferObject::GetContent(BufferAccess access) const
	{
		if (IsMaped())
		{
			auto result = m_Internals->Map.lock();

			if (result->Access() != access)
				GL_LOG_WARN("Buffer is already maped with another access rights");

			return result;
		}

		auto result = Pointer<BufferContent>(new BufferContent(access, this));
		m_Internals->Map = result;

		return result;
	}

	void * BufferObject::MapBuffer(BufferAccess access) const
	{
		return m_Internals->MapBuffer(access);
	}

	void BufferObject::UnMapBuffer() const
	{
		m_Internals->UnMapBuffer();
	}

	BufferContent::BufferContent(BufferAccess access, const BufferObject *object) : m_Buffer(object),
	                                                                                m_Access(access)
	{
		ASSERT(object->Size() != 0, "Attempting to get access in to uninitialized memory");

		if(object->Size() == 0)
			throw std::runtime_error("Attempting to get access in to uninitialized memory");

		m_Data = object->MapBuffer(access);

		if (m_Data)
		{
			GL_LOG_DEBUG("Sucessfuly maped buffer {}", object->ID());
		}
		else
		{
			GL_LOG_ERROR("Unable to map buffer, {}", object->ID());
		}
	}

	void BufferContent::Invalidate()
	{
		m_Buffer = nullptr;
		m_Data = nullptr;
	}

	BufferContent::~BufferContent()
	{
		m_Buffer->UnMapBuffer();
	}

	const void* BufferContent::Get() const
	{
		ASSERT(m_Access != BufferAccess::WriteOnly, "No read access");

		if(m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}

	void* BufferContent::Get()
	{
		ASSERT(m_Access != BufferAccess::WriteOnly, "No read access");

		if(m_Access == BufferAccess::WriteOnly)
			throw std::runtime_error("No read access");

		return m_Data;
	}

	void BufferContent::Set(const void *data, size_t size, size_t offset)
	{
		ASSERT(m_Data);
		if (!m_Data)
		{
			GL_LOG_ERROR("Empty data");

			return;
		}

		ASSERT(m_Access != BufferAccess::ReadOnly, "No write access");
		ASSERT(size + offset < m_Buffer->Size(), "Data povided is too bing");

		if(m_Access == BufferAccess::ReadOnly)
			throw std::runtime_error("No write access");

		if(size + offset > m_Buffer->Size())
			throw std::out_of_range("Data size provided is too bing");

		std::memcpy(static_cast<char*>(m_Data) + offset, data, size);
	}
}
