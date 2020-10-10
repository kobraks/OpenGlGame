#include "pch.h"
#include "Uniform.h"

#include "Assert.h"
#include "Log.h"
#include "ShaderProgram.h"

namespace Game
{
	UniformObject::~UniformObject()
	{
		free(m_Data);
	}

	UniformObject::UniformObject(ShaderProgram &program, const UniformDetails &details)
	{
		m_Program = &program;
		m_Details = details;

		InitializeData(details.Type);
	}

	UniformObject::UniformObject(const UniformObject &object)
	{
		*this = object;
	}

	UniformObject::UniformObject(UniformObject &&object) noexcept
	{
		*this = object;
	}

	UniformObject& UniformObject::operator=(const UniformObject &object)
	{
		m_Program = object.m_Program;
		m_Details = object.m_Details;
		m_Changed = object.m_Changed;
		
		m_DataSize = object.m_DataSize;

		m_Data = malloc(m_DataSize);
		memcpy_s(m_Data, m_DataSize, object.m_Data, object.m_DataSize);

		return *this;
	}

	UniformObject& UniformObject::operator=(UniformObject &&object) noexcept
	{
		m_Program  = object.m_Program;
		m_DataSize = object.m_DataSize;
		m_Changed = object.m_Changed;

		m_Details = std::move(object.m_Details);

		m_Data        = object.m_Data;
		object.m_Data = nullptr;

		return *this;
	}

	void UniformObject::SendData() const
	{
		m_Changed = false;
		
		switch(GetType())
		{
			case UniformType::Float:
				glUniform1fv(GetLocation(), GetCount(), static_cast<float*>(m_Data));
				break;
			case UniformType::Float2:
				glUniform2fv(GetLocation(), GetCount(), static_cast<float*>(m_Data));
				break;
			case UniformType::Float3:
				glUniform3fv(GetLocation(), GetCount(), static_cast<float*>(m_Data));
				break;
			case UniformType::Float4:
				glUniform4fv(GetLocation(), GetCount(), static_cast<float*>(m_Data));
				break;
			case UniformType::Int:
				glUniform1iv(GetLocation(), GetCount(), static_cast<int32_t*>(m_Data));
				break;
			case UniformType::Mat3X3:
				glUniformMatrix3fv(GetLocation(), GetCount(), GL_FALSE, static_cast<float*>(m_Data));
				break;
			case UniformType::Mat4X4:
				glUniformMatrix4fv(GetLocation(), GetCount(), GL_FALSE, static_cast<float*>(m_Data));
				break;
			default: GL_LOG_ERROR("Invalid data type data not send");
				break;
		}
	}

	bool UniformObject::SetData(const void *data, const size_t size, const size_t offset)
	{
		ASSERT(!data, "Uninitialized data provided");
		ASSERT(!(size + offset < m_DataSize), "Data provided is beyond data buffer");
		
		if (!data)
			return false;

		if (size + offset < m_DataSize)
		{
			memcpy_s(static_cast<char*>(m_Data) + offset, m_DataSize - offset, data,  size);
			MarkAsChanged();
			return true;
		}

		return false;
	}

	void UniformObject::InitializeData(const UniformType type)
	{
		m_Data     = malloc(TypeSize(type));
		m_DataSize = TypeSize(type);
	}
}
