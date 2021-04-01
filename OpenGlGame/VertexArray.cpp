#include "pch.h"
#include "VertexArray.h"

#include "GLCheck.h"

namespace Game
{
	static constexpr GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch(type)
		{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4: return GL_FLOAT;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4: return GL_INT;
			case ShaderDataType::Bool: return GL_BOOL;
		}
	}

	void VertexArray::Internals::SetIndexBuffer(const Pointer<Game::IndexBuffer> &indexBuffer)
	{
		GL_CHECK(glBindVertexArray(Id));
		indexBuffer->Bind();

		IndexBuffer = indexBuffer;
	}

	void VertexArray::Internals::AddVertexBuffer(const Pointer<VertexBuffer> &vertexBuffer)
	{
		ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout");

		if(vertexBuffer->GetLayout().GetElements().size() == 0)
			throw std::exception("Vertex Buffer has no layout");

		GL_CHECK(glBindVertexArray(Id));
		vertexBuffer->Bind();

		const auto &layout = vertexBuffer->GetLayout();
		for(const auto &element : layout)
		{
			switch(element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
					GL_CHECK(
					         glVertexAttribPointer(VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.
						         Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(element.Offset) )
					        );
					VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
					GL_CHECK(
					         glVertexAttribIPointer(VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), layout.
						         GetStride(), (const void*)(element.Offset) )
					        );
					VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for(uint8_t i = 0; i < count; ++i)
					{
						GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
						GL_CHECK(
						         glVertexAttribPointer(VertexBufferIndex, count, ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE :
							         GL_FALSE, layout.GetStride(), (const void*)(element.Offset + sizeof(float)* count * i) )
						        );
						GL_CHECK(glVertexAttribDivisor(VertexBufferIndex, 1));
						VertexBufferIndex++;
					}
				}
				default: ASSERT(false, "Unknown ShaderDataType");
					throw std::exception("Unknown shaderDataType");
			}
		}

		VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::Internals::Bind() const
	{
		GL_CHECK(glBindVertexArray(Id));
	}

	void VertexArray::Internals::Unbind() const
	{
		GL_CHECK(glBindVertexArray(0));
	}

	VertexArray::Internals::Internals(IdType id)
	{
		Id = id;
	}

	VertexArray::Internals::Internals()
	{
		glCreateVertexArrays(1, &Id);
	}

	VertexArray::Internals::~Internals()
	{
		if(Id != 0)
			glDeleteVertexArrays(1, &Id);
	}

	VertexArray::VertexArray(IdType id)
	{
		m_Internals = MakePointer<Internals>(id);
	}

	VertexArray::VertexArray()
	{
		m_Internals = MakePointer<Internals>();
	}

	VertexArray* VertexArray::GetDefault()
	{
		static VertexArray array(0);

		return &array;
	}
}
