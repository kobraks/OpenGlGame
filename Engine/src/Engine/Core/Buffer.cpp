#include "pch.h"
#include "Buffer.h"

#include "Engine/Core/BufferView.h"

#include <cstring>
#include <memory>

namespace Engine {
	Buffer::Buffer(SizeType size) {
		Allocate(size);
	}

	Buffer::Buffer(const BufferView& view) : Buffer(view.Data(), view.Size()) {
	}

	Buffer::Buffer(const void* data, SizeType size) {
		Allocate(size);

		if (data) {
			std::memcpy(m_Data.get(), data, size);
		}
	}

	Buffer::Buffer(const Buffer& other, SizeType offset) {
		const auto length = other.Size() - offset;

		ENGINE_ASSERT(offset <= other.Size());
		if (offset > other.Size())
			throw std::runtime_error("Buffer overflow");

		Allocate(length);
		std::memcpy(m_Data.get(), other.m_Data.get() + offset, length);
	}

	Buffer::Buffer(Buffer&& other) noexcept : m_Data(std::move(other.m_Data)), m_Size(std::move(other.m_Size)) {
		other.m_Size = 0;
		other.m_Data = nullptr;
	}

	Buffer::~Buffer() {
		Release();
	}

	Buffer& Buffer::operator=(const Buffer& other) {
		if (this != &other) {
			Allocate(other.m_Size);
			std::memcpy(m_Data.get(), other.m_Data.get(), other.m_Size);
		}

		return *this;
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept {
		if (this != &other) {
			m_Data = std::move(other.m_Data);
			m_Size = other.m_Size;

			other.m_Size = 0;
			other.m_Data = nullptr;
		}

		return *this;
	}

	bool Buffer::operator==(const Buffer& rth) const {
		return m_Size == rth.m_Size && std::memcmp(m_Data.get(), rth.m_Data.get(), m_Size) == 0;
	}

	Buffer Buffer::Copy(const void* data, SizeType size) {
		return Buffer(data, size);
	}

	Buffer Buffer::Copy(const Buffer& buffer) {
		return Buffer(buffer.Data(), buffer.Size());
	}

	Buffer Buffer::FromSpan(std::span<const std::byte> span) {
		return Buffer(span.data(), span.size_bytes());
	}

	void Buffer::Allocate(SizeType size) {
		if (size == 0) {
			m_Data.reset();
			m_Size = 0;
			return;
		}

		m_Data = std::make_unique<std::byte[]>(size);
		m_Size = size;
	}

	void Buffer::Resize(SizeType newSize) {
		if (newSize == m_Size)
			return;

		auto newData = std::make_unique<std::byte[]>(newSize);
		if (m_Data && m_Size > 0) {
			std::memcpy(newData.get(), m_Data.get(), std::min(m_Size, newSize));
		}

		m_Data = std::move(newData);
		m_Size = newSize;
	}

	void Buffer::Release() {
		m_Data.reset();
		m_Size = 0;
	}

	void Buffer::Fill(std::byte value) {
		if (m_Data) {
			std::memset(m_Data.get(), static_cast<unsigned char>(value), m_Size);
		} else {
			throw std::runtime_error("Buffer::Fill: Buffer is not allocated");
		}
	}

	BufferView Buffer::Slice(SizeType offset, SizeType length) const {
		return BufferView(*this, offset, length, true);
	}

	Buffer Buffer::Clone() const {
		return Buffer(m_Data.get(), m_Size);
	}

	void Buffer::Read(std::byte* destination, SizeType size, SizeType offset) const {
		ENGINE_ASSERT(destination != nullptr, "Buffer::Read: Destination pointer is null");
		if (destination == nullptr)
			throw std::invalid_argument("Buffer::Read: Destination pointer is null");

		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::runtime_error("Buffer::Read: Overflow");

		std::memcpy(destination, m_Data.get() + offset, size);
	}

	void Buffer::Write(const BufferView& buffer, SizeType offset) {
		Write(buffer.Data(), buffer.Size(), offset);
	}

	void Buffer::Write(const std::byte* data, SizeType size, SizeType offset) {
		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::runtime_error("Buffer::Write: Overflow");

		std::memcpy(m_Data.get() + offset, data, size);
	}
}
