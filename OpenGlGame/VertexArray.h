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
		IdType Id() const { return *m_Array; }

		void Bind() const;
		void UnBind() const;

		static VertexArray* GetDefault();

		void AddElementBuffer(const IndexBuffer &buffer) { return AddIndexBuffer(buffer); }
		void AddIndexBuffer(const IndexBuffer &buffer);

		template <typename Type>
		void AddBuffer(const VertexBuffer<Type> &buffer, const VertexBufferLayout &layout);

	private:
		void AddVertexBuffer(const BufferObject& buffer, const VertexBufferLayout& layout);
	};

	template <typename Type>
	void VertexArray::AddBuffer(const VertexBuffer<Type> &buffer, const VertexBufferLayout &layout)
	{
		AddVertexBuffer(buffer, layout);
	}
}
