#include "pch.h"
#include "StreamWriter.h"

#include <array>
#include <algorithm>

namespace Engine {
	bool StreamWriter::WriteBuffer(const BufferView& buffer, bool writeSize) {
		if (writeSize) {
			if (!WriteRaw<BufferView::SizeType>(buffer.Size()))
				return false;
		}

		return WriteData(buffer.Data(), buffer.Size());
	}

	bool StreamWriter::WriteFill(std::byte value, std::size_t size) {
		if (size == 0)
			return true;

		constexpr std::size_t chunkSize = 4096;
		std::array<std::byte, chunkSize> buffer{};
		buffer.fill(value);

		while (size > 0) {
			const std::size_t bytesToWrite = std::min(size, chunkSize);
			if (!WriteData(buffer.data(), bytesToWrite)) {
				return false;
			}
			size -= bytesToWrite;
		}

		return true;
	}

	bool StreamWriter::WriteString(const std::string &string) {
		uint64_t size = string.size();

		if (!WriteRaw<uint64_t>(size))
			return false;

		return WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}

	bool StreamWriter::WriteString(std::string_view string) {
		uint64_t size = string.size();

		if (!WriteRaw<uint64_t>(size))
			return false;

		return WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string_view::value_type) * size);
	}
}
