#include "pch.h"
#include "StreamWriter.h"

namespace Engine {
	void StreamWriter::WriteBuffer(Buffer buffer, bool writeSize) {
		if (writeSize) {
			uint32_t size = static_cast<uint32_t>(buffer.Size);
			WriteData(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
		}

		WriteData(static_cast<const char*>(buffer.Data), buffer.Size);
	}

	void StreamWriter::WriteZero(size_t size) {
		char zero = 0;

		for (uint32_t i = 0; i < size; ++i)
			WriteData(&zero, 1);
	}

	void StreamWriter::WriteString(const std::string &string) {
		size_t size = string.size();

		WriteData(reinterpret_cast<const char*>(&size), sizeof(size_t));
		WriteData(reinterpret_cast<const char *>(string.data()), sizeof(std::string::value_type) * size);
	}

	void StreamWriter::WriteString(std::string_view string) {
		size_t size = string.size();

		WriteData(reinterpret_cast<const char*>(&size), sizeof(size_t));
		WriteData(reinterpret_cast<const char*>(string.data()), sizeof(std::string_view::value_type) * size);
	}
}
