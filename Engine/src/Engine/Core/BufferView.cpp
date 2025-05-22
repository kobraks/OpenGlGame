#include "pch.h"
#include "BufferView.h"
#include "Buffer.h"

namespace Engine {
	BufferView::BufferView(std::byte* data, SizeType size) : m_Data(data), m_Size(size) {
	}

	BufferView::BufferView(const void* data, SizeType size) : m_Data(
		const_cast<std::byte*>(static_cast<const std::byte*>(data))), m_Size(size) {
	}

	BufferView::BufferView(const Buffer& buffer, SizeType offset) {
		ENGINE_ASSERT(offset <= buffer.m_Size);
		if (offset > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(buffer.Data() + offset);
		m_Size = buffer.m_Size - offset;
	}

	BufferView::BufferView(const Buffer& buffer, SizeType offset, SizeType length) {
		ENGINE_ASSERT(offset + length <= buffer.m_Size)
		if (offset + length > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		// Iam creating a non-owning, potentially mutable view from const data
		// Mutability of the view must be respected externally
		m_Data = const_cast<std::byte*>(buffer.Data() + offset);
		m_Size = length;
	}

	BufferView::BufferView(const BufferView& buffer, SizeType offset) {
		ENGINE_ASSERT(offset <= buffer.m_Size);
		if (offset > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = buffer.m_Size - offset;
	}

	BufferView::BufferView(const BufferView& buffer, SizeType offset, SizeType length) {
		ENGINE_ASSERT(offset + length <= buffer.m_Size);
		if (offset + length > buffer.m_Size)
			throw std::runtime_error("Buffer overflow!");

		m_Data = const_cast<std::byte*>(static_cast<const std::byte*>(buffer.m_Data) + offset);
		m_Size = length;
	}

	std::span<std::byte> BufferView::AsSpan() {
		return { m_Data, m_Size };
	}

	std::span<const std::byte> BufferView::AsSpan() const {
		return { m_Data, m_Size };
	}

	BufferView BufferView::Slice(SizeType offset, SizeType length) const {
		return BufferView(*this, offset, length);
	}

}
