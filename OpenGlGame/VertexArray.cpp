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
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindVertexArray(Id));
		indexBuffer->Bind();

		OPENGL_LOG_DEBUG("Adding Index buffer (id: {}) to vertex arrray (id: {})", indexBuffer->Id(), Id);

		IndexBuffer = indexBuffer;
	}

	void VertexArray::Internals::AddVertexBuffer(const Pointer<VertexBuffer> &vertexBuffer)
	{
		CHECK_IF_VALID_CONTEXT;
		ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout");

		if(vertexBuffer->GetLayout().GetElements().size() == 0)
			throw std::exception("Vertex Buffer has no layout");

		GL_CHECK(glBindVertexArray(Id));
		vertexBuffer->Bind();

		OPENGL_LOG_DEBUG("Adding Vertex buffer (id: {}) to vertex array (id: {})", vertexBuffer->Id(), Id);

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
					OPENGL_LOG_DEBUG("Enabling vertex attrib array: {}", VertexBufferIndex);
					GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
					GL_CHECK(
					         glVertexAttribPointer(VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.
						         Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(element.Offset) )
					        );
					OPENGL_LOG_DEBUG(
					                 "Float Index: {}, element count: {}, DataType: {}, Normalized: {}, Stride: {}, Offset: {}",
					                 VertexBufferIndex,
					                 element.GetComponentCount(),
					                 element.Type,
					                 element.Normalized,
					                 layout.GetStride(),
					                 element.Offset
					                )

					VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					OPENGL_LOG_DEBUG("Enabling vertex attrib array: {}", VertexBufferIndex);

					GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
					GL_CHECK(
					         glVertexAttribIPointer(VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), layout.
						         GetStride(), (const void*)(element.Offset) )
					        );


					OPENGL_LOG_DEBUG(
					                 "Int Index: {}, element count: {}, DataType: {}, Stride: {}, Offset: {}",
					                 VertexBufferIndex,
					                 element.GetComponentCount(),
					                 element.Type,
					                 layout.GetStride(),
					                 element.Offset
					                )

					VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for(uint8_t i = 0; i < count; ++i)
					{
						OPENGL_LOG_DEBUG("Enabling vertex attrib array: {}", VertexBufferIndex);
						GL_CHECK(glEnableVertexAttribArray(VertexBufferIndex));
						GL_CHECK(
						         glVertexAttribPointer(VertexBufferIndex, count, ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE :
							         GL_FALSE, layout.GetStride(), (const void*)(element.Offset + sizeof(float)* count * i) )
						        );
						GL_CHECK(glVertexAttribDivisor(VertexBufferIndex, 1));

						OPENGL_LOG_DEBUG(
						                 "Matrix Index: {}, element count: {}, DataType: {}, Normalized: {}, Stride: {}, Offset: {}",
						                 VertexBufferIndex,
						                 count,
						                 element.Type,
						                 element.Normalized,
						                 layout.GetStride(),
						                 element.Offset + sizeof(float)* count * i
						                )
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
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindVertexArray(Id));
	}

	void VertexArray::Internals::Unbind() const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindVertexArray(0));
	}

	VertexArray::Internals::Internals(IdType id)
	{
		Id = id;
	}

	VertexArray::Internals::Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		glCreateVertexArrays(1, &Id);
		OPENGL_LOG_DEBUG("Creating vertex array: {}", Id);
	}

	VertexArray::Internals::~Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		if(Id != 0)
			glDeleteVertexArrays(1, &Id);
		OPENGL_LOG_DEBUG("Removing vertex array: {}", Id);
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
