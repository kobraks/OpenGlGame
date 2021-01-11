#include "pch.h"
#include "VertexArray.h"

#include <glad/glad.h>

#include "GLCheck.h"

namespace Game
{
	static void DeleteArray(VertexArray::IdType *id)
	{
		GL_CHECK(glDeleteVertexArrays(1, id));
		delete id;
	}

	static Pointer<VertexArray::IdType> CreateArray()
	{
		auto array = Pointer<VertexArray::IdType>(new VertexArray::IdType{}, DeleteArray);
		GL_CHECK(glGenVertexArrays(1, &*array));

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
		GL_CHECK(glBindVertexArray(*m_Array));
	}

	void VertexArray::UnBind() const
	{
		GetDefault()->Bind();
	}

	VertexArray* VertexArray::GetDefault()
	{
		static VertexArray array(0);

		return &array;
	}

	void VertexArray::AddIndexBuffer(const IndexBuffer &buffer)
	{
		Bind();
		buffer.Bind();
	}

	void VertexArray::AddVertexBuffer(const BufferObject &buffer, const VertexBufferLayout &layout)
	{
		GL_LOG_INFO("Adding buffer {} to vertex array: {}", buffer.ID(), ID());
		Bind();
		buffer.Bind();
		const auto &elements = layout.GetElements();
		uint32_t offset      = 0;

		for(uint32_t i = 0; i < elements.size(); ++i)
		{
			const auto &element = elements[i];
			GL_CHECK(glEnableVertexAttribArray(i));

			GL_LOG_DEBUG("Enabling atribute array {}", i);
			GL_CHECK(glVertexAttribPointer(
			                      i,
			                      static_cast<GLint>(element.Count),
			                      static_cast<GLenum>(element.Type),
			                      element.Normalized ? GL_TRUE : GL_FALSE,
			                      static_cast<GLsizei>(layout.GetStride()),
			                      reinterpret_cast<const void*>(offset)
			                     ));

			GL_LOG_DEBUG(
			             "Element: (Count: {}, Type: {}, Normalized: {}, Offset: {}, Stride: {})",
			             element.Count,
			             element.Type,
			             element.Normalized,
			             offset,
			             layout.GetStride()
			            );

			offset += element.Count * element.Size;
		}
	}
}
