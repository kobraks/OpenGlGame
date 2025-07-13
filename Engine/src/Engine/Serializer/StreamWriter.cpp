#include "pch.h"
#include "StreamWriter.h"

namespace Engine {
	void StreamWriter::WriteBuffer(Buffer buffer, bool writeSize) {
		if (writeSize) {
			const auto size = buffer.Size();
			WriteData(reinterpret_cast<const std::byte*>(&size), sizeof(Buffer::SizeType));
		}

		WriteData(buffer.Data(), buffer.Size());
	}

	void StreamWriter::WriteZero(std::size_t size) {
		constexpr std::byte zero(static_cast<std::byte>(0));

		for (std::size_t i = 0; i < size; ++i)
			WriteData(&zero, 1);
	}

	void StreamWriter::WriteString(const std::string &string) {
		std::size_t size = string.size();

		WriteData(reinterpret_cast<std::byte*>(&size), sizeof(std::size_t));
		WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}

	void StreamWriter::WriteString(std::string_view string) {
		size_t size = string.size();

		WriteData(reinterpret_cast<std::byte*>(&size), sizeof(size_t));
		WriteData(reinterpret_cast<const std::byte*>(string.data()), sizeof(std::string_view::value_type) * size);
	}
}
