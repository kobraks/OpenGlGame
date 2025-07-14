#include "pch.h"
#include "FileStream.h"

namespace Engine {
	FileStreamWriter::FileStreamWriter(const std::filesystem::path &path) : m_Path(path) {
		m_Stream = std::ofstream(path, std::ifstream::out | std::ifstream::binary);
	}

	FileStreamWriter::~FileStreamWriter() {
		m_Stream.close();
	}

	bool FileStreamWriter::WriteData(const std::byte *data, size_t size) {
		m_Stream.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
		return !StreamFailed();
	}

	bool FileStreamWriter::StreamFailed() const {
		return !m_Stream || m_Stream.fail();
	}

	FileStreamReader::FileStreamReader(const std::filesystem::path &path) : m_Path(path) {
		m_Stream = std::ifstream(path, std::ifstream::in | std::ifstream::binary);
	}

	FileStreamReader::~FileStreamReader() {
		m_Stream.close();
	}

	bool FileStreamReader::ReadData(std::byte *destination, size_t size) {
		m_Stream.read(reinterpret_cast<char*>(destination), static_cast<std::streamsize>(size));
		return !StreamFailed();
	}

	bool FileStreamReader::StreamFailed() const {
		return !m_Stream || m_Stream.fail() || m_Stream.eof();
	}
}
