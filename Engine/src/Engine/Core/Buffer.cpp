#include "pch.h"
#include "Buffer.h"

#include <cstring>
#include <memory>

namespace Engine {
	Buffer::Buffer() : Data(nullptr),
	                   Size(0) {}

	Buffer::Buffer(void *data, SizeType size) : Data(data),
	                                          Size(size) {}

	Buffer::Buffer(const Buffer &other, SizeType size) : Data(other.Data),
	                                                   Size(size) {}

	Buffer::Buffer(const Buffer& buffer) = default;
	Buffer& Buffer::operator=(const Buffer& buffer) = default;

	Buffer::Buffer(Buffer&& buffer) noexcept {
		Data = std::exchange(buffer.Data, nullptr);
		Size = std::exchange(buffer.Size, 0);
	}

	Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
		Release();
		Data = std::exchange(buffer.Data, nullptr);
		Size = std::exchange(buffer.Size, 0);

		return *this;
	}

	Buffer Buffer::Copy(const Buffer &other) {
		Buffer buffer;

		buffer.Allocate(other.Size);
		std::memcpy(buffer.Data, other.Data, other.Size);

		return buffer;
	}

	Buffer Buffer::Copy(const void *data, SizeType size) {
		Buffer buffer;

		buffer.Allocate(size);
		std::memcpy(buffer.Data, data, size);

		return buffer;
	}

	void Buffer::Allocate(SizeType size) {
		delete[] static_cast<uint8_t*>(Data);
		Data = nullptr;
		Size = size;

		if(size == 0)
			return;

		Data = new uint8_t[size];
	}

	void Buffer::Release() {
		delete[] static_cast<uint8_t*>(Data);

		Data = nullptr;
		Size = 0;
	}

	void Buffer::ZeroInitialize() {
		if(Data)
			std::memset(Data, 0, Size);
	}

	uint8_t * Buffer::ReadBytes(SizeType size, SizeType offset) const {
		ENGINE_ASSERT(offset + size <= Size, "Buffer overflow!");

		auto buffer = new uint8_t[size];

		memcpy(buffer, static_cast<uint8_t*>(Data) + offset, size);
		return buffer;
	}

	void Buffer::Write(const void *data, SizeType size, SizeType offset) {
		ENGINE_ASSERT((offset + size) <= Size, "Buffer overflow!");

		memcpy(static_cast<uint8_t*>(Data) + offset, data, size);
	}

	Buffer::operator bool() const {
		return Data;
	}

	uint8_t & Buffer::operator[](SizeType idx) {
		ENGINE_ASSERT(idx < Size);
		if (idx >= Size)
			throw std::out_of_range("Buffer: index out of range.");

		return static_cast<uint8_t*>(Data)[idx];
	}

	uint8_t Buffer::operator[](SizeType idx) const {
		ENGINE_ASSERT(idx < Size);
		if (idx >= Size)
			throw std::out_of_range("Buffer: index out of range.");

		return static_cast<uint8_t*>(Data)[idx];
	}
}
