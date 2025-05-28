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

	bool BufferView::operator==(const BufferView& rth) const {
		return m_Size == rth.m_Size && std::memcmp(m_Data, rth.m_Data, m_Size) == 0;
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
		return BufferView(*this, offset, length);
	}

}
