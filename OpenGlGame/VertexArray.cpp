#include "pch.h"
#include "VertexArray.h"

#include <glad/glad.h>

namespace Game
{
	static void DeleteArray(VertexArray::IdType* id)
	{
		glDeleteVertexArrays(1, id);
		delete id;
	}

	static Pointer<VertexArray::IdType> CreateArray()
	{
		auto array = Pointer<VertexArray::IdType>(new VertexArray::IdType{}, DeleteArray);
		glGenVertexArrays(1, &*array);

		return array;
	}
	
	VertexArray::VertexArray(IdType id)
	{
		m_Array = MakePointer<IdType>(0);
	}

	VertexArray::VertexArray()
	{
		m_Array = CreateArray();
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(*m_Array);
	}
	
	void VertexArray::UnBind() const
	{
		GetDefault()->Bind();
	}

	VertexArray * VertexArray::GetDefault()
	{
		static VertexArray array(0);

		return &array;
	}

	void VertexArray::AddIndexBuffer(const IndexBuffer &buffer)
	{
		Bind();
		buffer.Bind();
	}
}
