#include "pch.h"
#include "BufferView.h"
#include "Buffer.h"

namespace Engine {
	BufferView::BufferView(std::byte* data, SizeType size, bool allowWrite) : m_Data(data), m_Size(size), m_AllowWrite(allowWrite) {
	}

	BufferView::BufferView(const void* data, SizeType size, bool allowWrite) : m_Data(
		const_cast<std::byte*>(static_cast<const std::byte*>(data))), m_Size(size), m_AllowWrite(allowWrite) {
	}

	BufferView::BufferView(const Buffer& buffer, SizeType offset, bool allowWrite) {
		ENGINE_ASSERT(offset <= buffer.m_Size);
		if (offset > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(buffer.Data() + offset);
		m_Size = buffer.m_Size - offset;
		m_AllowWrite = allowWrite;
	}

	BufferView::BufferView(const Buffer& buffer, SizeType offset, SizeType length, bool allowWrite) {
		ENGINE_ASSERT(offset + length <= buffer.m_Size)
		if (offset + length > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(buffer.Data() + offset);
		m_Size = length;
		m_AllowWrite = allowWrite;
	}

	BufferView::BufferView(const BufferView& buffer, SizeType offset, bool allowWrite) {
		ENGINE_ASSERT(offset <= buffer.m_Size);
		if (offset > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = buffer.m_Size - offset;
		m_AllowWrite = allowWrite;
	}

	BufferView::BufferView(const BufferView& buffer, SizeType offset, SizeType length, bool allowWrite) {
		ENGINE_ASSERT(offset + length <= buffer.m_Size);
		if (offset + length > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = length;
		m_AllowWrite = allowWrite;
	}

	bool BufferView::operator==(const BufferView& rth) const {
		return m_Size == rth.m_Size && m_AllowWrite == rth.m_AllowWrite && std::memcmp(m_Data, rth.m_Data, m_Size) == 0;
	}

	std::span<std::byte> BufferView::AsSpan(SizeType offset) {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		return { m_Data + offset, m_Size - offset };
	}

	std::span<const std::byte> BufferView::AsSpan(SizeType offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		return { m_Data + offset, m_Size - offset };
	}

	std::span<std::byte> BufferView::AsSpan(SizeType count, SizeType offset) {
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	std::span<const std::byte> BufferView::AsSpan(SizeType count, SizeType offset) const {
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	BufferView BufferView::Slice(SizeType offset, SizeType length) const {
		return BufferView(*this, offset, length, m_AllowWrite);
	}

	void BufferView::Write(const std::byte* data, SizeType size, SizeType offset) {
		ENGINE_ASSERT(m_AllowWrite, "BufferView::Write: Attempting to write to a read-only buffer view.");
		if (!m_AllowWrite)
			throw std::runtime_error("BufferView::Write: Attempting to write to a read-only buffer view.");

		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::out_of_range(fmt::format("BufferView::Write: Out of bounds (offset={}, size={}, buffer size={})", offset, size, m_Size));

		std::memcpy(m_Data + offset, data, size);
	}

	void BufferView::Fill(std::byte value) {
		ENGINE_ASSERT(m_AllowWrite, "BufferView::Fill: Attempting to fill a read-only buffer view.");
		if (!m_AllowWrite)
			throw std::runtime_error("BufferView::Fill: Attempting to fill a read-only buffer view.");

		if (m_Data)
			std::memset(m_Data, static_cast<unsigned char>(value), m_Size);
		else
			throw std::runtime_error("BufferView::Fill: Cannot fill an empty buffer view.");
	}

	void BufferView::Write(const BufferView& buffer, SizeType offset) {
		ENGINE_ASSERT(m_AllowWrite, "BufferView::Write: Attempting to write to a read-only buffer view.");
		if (!m_AllowWrite)
			throw std::runtime_error("BufferView::Write: Attempting to write to a read-only buffer view.");

		ENGINE_ASSERT(offset + buffer.Size() <= m_Size);
		if (offset + buffer.Size() > m_Size)
			throw std::out_of_range(fmt::format("BufferView::Write: Out of bounds (offset={}, size={}, buffer size={})", offset, buffer.Size(), m_Size));

		std::memcpy(m_Data + offset, buffer.Data(), buffer.Size());
	}
}
