#pragma once
#include "Engine/OpenGL/BufferObject.h"

namespace Game
{
	class IndexBuffer : public BufferObject
	{
		struct Internals
		{
			size_t Count = 0;
		};

		Pointer<Internals> m_Internals;

	public:
		IndexBuffer(const uint32_t *indices, size_t count);

		size_t Count() const { return m_Internals->Count; }
	};
}