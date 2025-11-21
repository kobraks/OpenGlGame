#include "pch.h"
#include "StreamWriter.h"

#include <array>
#include <algorithm>

#include "Engine/Core/Buffer.h"

namespace Engine {
	bool StreamWriter::WriteBuffer(const BufferView& buffer, bool writeSize) {
		if (writeSize) {
			if (!WriteRaw<Stream::BufferSize>(static_cast<Stream::BufferSize>(buffer.Size())))
				return false;
		}

		return WriteData(buffer.Data(), buffer.Size());
	}

	bool StreamWriter::WriteFill(std::byte value, Stream::SizeType size) {
		if (size == 0)
			return true;

		constexpr Stream::BufferSize chunkSize = 4096;
		std::array<std::byte, chunkSize> buffer{};
		buffer.fill(value);

		while (size > 0) {
			const auto bytesToWrite = std::min(size, chunkSize);
			if (!WriteData(buffer.data(), bytesToWrite)) {
				return false;
			}
			size -= bytesToWrite;
		}

		return true;
	}

	bool StreamWriter::WriteString(const std::string &string) {
		Stream::StringSize size = static_cast<Stream::StringSize>(string.size());

		if (!WriteRaw<Stream::StringSize>(size))
			return false;

		return WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}

	bool StreamWriter::WriteString(std::string_view string) {
		Stream::StringSize size = static_cast<Stream::StringSize>(string.size());

		if (!WriteRaw<Stream::StringSize>(size))
			return false;

		return WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string_view::value_type) * size);
	}
}
