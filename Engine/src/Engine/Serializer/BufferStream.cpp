#include "pch.h"
#include "Engine/Serializer/BufferStream.h"

namespace Engine {
	BufferStreamWriter::BufferStreamWriter(Buffer targetBuffer, size_t position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position) {}

	bool BufferStreamWriter::WriteData(const char *data, size_t size) {
		bool valid = m_BufferPosition + size <= m_TargetBuffer.Size;

		ENGINE_ASSERT(valid)
		if(!valid)
			return false;

		memcpy(m_TargetBuffer.As<uint8_t>() + m_BufferPosition, data, size);
		m_BufferPosition += size;
		return true;
	}

	BufferStreamReader::BufferStreamReader(Buffer targetBuffer, size_t position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position) {}

	bool BufferStreamReader::ReadData(char *destination, size_t size) {
		bool valid = m_BufferPosition + size <= m_TargetBuffer.Size;

		ENGINE_ASSERT(valid);
		if (!valid)
			return false;

		memcpy(destination, m_TargetBuffer.As<uint8_t>() + m_BufferPosition, size);
		return true;
	}
}
