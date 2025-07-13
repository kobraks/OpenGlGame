#include "pch.h"
#include "Engine/Serializer/BufferStream.h"

namespace Engine {
	BufferStreamWriter::BufferStreamWriter(Buffer targetBuffer, Buffer::SizeType position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position) {}

	bool BufferStreamWriter::WriteData(const std::byte *data, std::size_t size) {
		const bool valid = m_BufferPosition + size <= m_TargetBuffer.Size();

		ENGINE_ASSERT(valid)
		if(!valid)
			return false;

		std::memcpy(m_TargetBuffer.Data() + m_BufferPosition, data, size);
		m_BufferPosition += size;
		return true;
	}

	BufferStreamReader::BufferStreamReader(Buffer targetBuffer, Buffer::SizeType position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position) {}

	bool BufferStreamReader::ReadData(std::byte *destination, std::size_t size) {
		const bool valid = m_BufferPosition + size <= m_TargetBuffer.Size();

		ENGINE_ASSERT(valid);
		if (!valid)
			return false;

		std::memcpy(destination, m_TargetBuffer.Data() + m_BufferPosition, size);
		return true;
	}
}
