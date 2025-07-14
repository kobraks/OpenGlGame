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

	BufferView::BufferView(const BufferView& buffer, SizeType offset) {
		ENGINE_ASSERT(offset <= buffer.m_Size);
		if (offset > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = buffer.m_Size - offset;
		m_AllowWrite = buffer.m_AllowWrite;
	}

	BufferView::BufferView(const BufferView& buffer, SizeType offset, SizeType length) {
		ENGINE_ASSERT(offset + length <= buffer.m_Size);
		if (offset + length > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = length;
		m_AllowWrite = buffer.m_AllowWrite;
	}

	bool BufferView::operator==(const BufferView& rth) const {
		return m_Size == rth.m_Size && m_AllowWrite == rth.m_AllowWrite && std::memcmp(m_Data, rth.m_Data, m_Size) == 0;
	}

	BufferView BufferView::Slice(SizeType offset, SizeType length) const {
		return BufferView(*this, offset, length);
	}

	void BufferView::Read(std::byte* destination, SizeType size, SizeType offset) const {
		ENGINE_ASSERT(destination != nullptr, "BufferView::Read: Destination pointer is null");
		if (destination == nullptr)
			throw std::invalid_argument("BufferView::Read: Destination pointer is null");

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::Read: Out of bounds (offset={}, size={}, buffer size={})", offset, size, m_Size));

		std::memcpy(destination, m_Data + offset, size);
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

	void BufferView::Write(const std::byte* data, SizeType size, SizeType offset) {
		ENGINE_ASSERT(m_AllowWrite, "BufferView::Write: Attempting to write to a read-only buffer view.");
		if (!m_AllowWrite)
			throw std::runtime_error("BufferView::Write: Attempting to write to a read-only buffer view.");

		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::out_of_range(fmt::format("BufferView::Write: Out of bounds (offset={}, size={}, buffer size={})", offset, size, m_Size));

		std::memcpy(m_Data + offset, data, size);
	}
}
