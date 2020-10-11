#pragma once

#include <glad/glad.h>

#include "Types.h"
#include "Log.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

namespace Game
{
	class VertexArray
	{
	public:
		using IdType = uint32_t;

	private:
		Pointer<IdType> m_Array;

		VertexArray(IdType id);
	public:
		VertexArray();

		operator IdType() const { return *m_Array; }
		IdType ID() const { return *m_Array; }

		void Bind() const;
		void UnBind() const;

		static VertexArray* GetDefault();

		void AddElementBuffer(const IndexBuffer &buffer) { return AddIndexBuffer(buffer); }
		void AddIndexBuffer(const IndexBuffer &buffer);

		template <typename Type>
		void AddBuffer(const VertexBuffer<Type> &buffer, const VertexBufferLayout &layout);
	};

	template <typename Type>
	void VertexArray::AddBuffer(const VertexBuffer<Type> &vertexBuffer, const VertexBufferLayout &layout)
	{
		GL_LOG_INFO("Adding buffer {} to vertex array: {}", vertexBuffer.ID(), ID());
		Bind();
		vertexBuffer.Bind();
		const auto &elements = layout.GetElements();
		uint32_t offset      = 0;

		for(size_t i = 0; i < elements.size(); ++i)
		{
			const auto &element = elements[i];
			glEnableVertexAttribArray(i);

			GL_LOG_DEBUG("Enabling atribute array {}", i);

			glVertexAttribPointer(
			                      i,
			                      element.Count,
			                      element.Type,
			                      element.Normalized ? GL_TRUE : GL_FALSE,
			                      layout.GetStride(),
			                      reinterpret_cast<const void*>(offset)
			                     );

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
