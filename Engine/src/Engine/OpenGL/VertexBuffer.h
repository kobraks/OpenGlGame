#pragma once
#include "Engine/OpenGL/BufferObject.h"
#include "Engine/OpenGL/BufferLayout.h"

namespace Game
{
	class VertexBuffer: public BufferObject
	{
		struct Internals
		{
			BufferLayout Layout;
		};

		Pointer<Internals> m_Internals;
	public:
		VertexBuffer(size_t size, BufferUsage usage = BufferUsage::DynamicDraw);
		VertexBuffer(const void *vertices, size_t size, BufferUsage usage = BufferUsage::DynamicDraw);

		void SetData(const void *data, size_t size, size_t offset);

		void Realloc(size_t size, const void *data = nullptr, BufferUsage usage = BufferUsage::DynamicDraw);

		const BufferLayout& Layout() const { return m_Internals->Layout; }

		void Layout(const BufferLayout &layout) { m_Internals->Layout = layout; }
		void Layout(BufferLayout &&layout) { m_Internals->Layout = std::move(layout); }
	};
}
