#include "pch.h"
#include "Engine/OpenGl/IndexBuffer.h"

namespace Game
{
	IndexBuffer::IndexBuffer(const uint32_t *indices, size_t count) : BufferObject(BufferType::Index)
	{
		m_Internals = MakePointer<Internals>();
		m_Internals->Count = count;
		Data(BufferUsage::StaticDraw, count * sizeof(uint32_t), indices);
	}
}