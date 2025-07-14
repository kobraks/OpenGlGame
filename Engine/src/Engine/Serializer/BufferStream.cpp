#include "pch.h"
#include "Engine/Serializer/BufferStream.h"

namespace Engine {
	BufferStreamWriter::BufferStreamWriter(BufferView& targetBuffer, BufferView::SizeType position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position), m_BufferBeginPosition(position) {
		ENGINE_ASSERT(m_TargetBuffer.IsWritable(), "BufferStreamWriter: Target buffer must be writable.");
	}

	bool BufferStreamWriter::WriteData(const std::byte *data, std::size_t size) {
		const bool valid = m_BufferPosition + size <= m_TargetBuffer.Size();

		ENGINE_ASSERT(valid)
		if(!valid)
			return false;

		m_TargetBuffer.Write(data, size, m_BufferPosition);
		m_BufferPosition += size;

		return true;
	}

	BufferStreamReader::BufferStreamReader(BufferView& targetBuffer, BufferView::SizeType position) : m_TargetBuffer(targetBuffer),
		m_BufferPosition(position), m_BufferBeginPosition(position) {}

	bool BufferStreamReader::ReadData(std::byte *destination, std::size_t size) {
		const bool valid = m_BufferPosition + size <= m_TargetBuffer.Size();

		ENGINE_ASSERT(valid);
		if (!valid)
			return false;

		m_TargetBuffer.Read(destination, size, m_BufferPosition);
		m_BufferPosition += size;

		return true;
	}
}
